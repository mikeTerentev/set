import json
import os
from typing import Optional

STATE_FILE = os.path.join(os.path.dirname(__file__), "home_state.json")


class HomeManager:
    def __init__(self):
        self._load()

    def _load(self):
        with open(STATE_FILE, "r", encoding="utf-8") as f:
            self._state = json.load(f)

    def _save(self):
        with open(STATE_FILE, "w", encoding="utf-8") as f:
            json.dump(self._state, f, ensure_ascii=False, indent=2)

    # ── Getters ───────────────────────────────────────────────────────────────

    def get_all_devices(self) -> dict:
        """Return full state snapshot."""
        return json.loads(json.dumps(self._state))

    def get_lights(self) -> list:
        return self._state["lights"]

    def get_blinds(self) -> list:
        return self._state["blinds"]

    def _find_light(self, light_id: str) -> Optional[dict]:
        for light in self._state["lights"]:
            if light["id"] == light_id:
                return light
        return None

    def _find_blind(self, blind_id: str) -> Optional[dict]:
        for blind in self._state["blinds"]:
            if blind["id"] == blind_id:
                return blind
        return None

    # ── Light controls ────────────────────────────────────────────────────────

    def set_light(
        self,
        light_id: str,
        on: Optional[bool] = None,
        brightness: Optional[int] = None,
        color: Optional[str] = None,
    ) -> dict:
        light = self._find_light(light_id)
        if light is None:
            raise ValueError(f"Свет с id '{light_id}' не найден")

        if on is not None:
            light["on"] = on
        if brightness is not None:
            if not 0 <= brightness <= 100:
                raise ValueError("Яркость должна быть от 0 до 100")
            light["brightness"] = brightness
        if color is not None:
            light["color"] = color

        self._save()
        return dict(light)

    def set_room_lights(
        self,
        room: str,
        on: Optional[bool] = None,
        brightness: Optional[int] = None,
        color: Optional[str] = None,
    ) -> list:
        updated = []
        for light in self._state["lights"]:
            if light["room"].lower() == room.lower():
                if on is not None:
                    light["on"] = on
                if brightness is not None:
                    light["brightness"] = brightness
                if color is not None:
                    light["color"] = color
                updated.append(dict(light))
        if not updated:
            raise ValueError(f"Комната '{room}' не найдена или в ней нет освещения")
        self._save()
        return updated

    # ── Blind controls ────────────────────────────────────────────────────────

    def set_blind(self, blind_id: str, position: int) -> dict:
        blind = self._find_blind(blind_id)
        if blind is None:
            raise ValueError(f"Жалюзи с id '{blind_id}' не найдены")
        if not 0 <= position <= 100:
            raise ValueError("Позиция должна быть от 0 (закрыты) до 100 (открыты)")
        blind["position"] = position
        self._save()
        return dict(blind)

    def set_room_blinds(self, room: str, position: int) -> list:
        updated = []
        for blind in self._state["blinds"]:
            if blind["room"].lower() == room.lower():
                blind["position"] = position
                updated.append(dict(blind))
        if not updated:
            raise ValueError(f"Комната '{room}' не найдена или в ней нет жалюзи")
        self._save()
        return updated

    # ── Scenes ────────────────────────────────────────────────────────────────

    def all_off(self) -> dict:
        """Turn off all lights, close all blinds."""
        for light in self._state["lights"]:
            light["on"] = False
        for blind in self._state["blinds"]:
            blind["position"] = 0
        self._save()
        return {"message": "Всё выключено, жалюзи закрыты"}
