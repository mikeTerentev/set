"""
Tool definitions for Claude and their execution against HomeManager.
"""

import json
from devices import HomeManager

# ── Tool schemas ──────────────────────────────────────────────────────────────

TOOLS = [
    {
        "name": "list_devices",
        "description": (
            "Возвращает полный список устройств умного дома: всех светильников и жалюзи "
            "с их текущим состоянием (включён/выключен, яркость, цвет, позиция)."
        ),
        "input_schema": {
            "type": "object",
            "properties": {},
            "required": [],
        },
    },
    {
        "name": "set_light",
        "description": (
            "Управляет одним светильником по его id. "
            "Можно менять: включение/выключение, яркость (0–100), цвет (hex, например #FF5500)."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "light_id": {
                    "type": "string",
                    "description": "ID светильника (например light_living_main)",
                },
                "on": {
                    "type": "boolean",
                    "description": "true — включить, false — выключить",
                },
                "brightness": {
                    "type": "integer",
                    "description": "Яркость от 0 до 100",
                },
                "color": {
                    "type": "string",
                    "description": "Цвет в формате HEX, например #FFFFFF",
                },
            },
            "required": ["light_id"],
        },
    },
    {
        "name": "set_room_lights",
        "description": (
            "Управляет всеми светильниками в указанной комнате одновременно. "
            "Комнаты: Гостиная, Спальня, Кухня, Коридор."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "room": {
                    "type": "string",
                    "description": "Название комнаты: Гостиная, Спальня, Кухня, Коридор",
                },
                "on": {"type": "boolean", "description": "true — включить, false — выключить"},
                "brightness": {"type": "integer", "description": "Яркость от 0 до 100"},
                "color": {"type": "string", "description": "Цвет HEX, например #FFF5E0"},
            },
            "required": ["room"],
        },
    },
    {
        "name": "set_blind",
        "description": (
            "Управляет одними жалюзи по их id. "
            "Позиция 0 — полностью закрыты, 100 — полностью открыты."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "blind_id": {
                    "type": "string",
                    "description": "ID жалюзи (например blind_living_left)",
                },
                "position": {
                    "type": "integer",
                    "description": "Позиция от 0 (закрыты) до 100 (открыты)",
                },
            },
            "required": ["blind_id", "position"],
        },
    },
    {
        "name": "set_room_blinds",
        "description": (
            "Управляет всеми жалюзи в указанной комнате. "
            "Позиция 0 — закрыты, 100 — открыты."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "room": {
                    "type": "string",
                    "description": "Название комнаты: Гостиная, Спальня, Кухня, Коридор",
                },
                "position": {
                    "type": "integer",
                    "description": "Позиция от 0 (закрыты) до 100 (открыты)",
                },
            },
            "required": ["room", "position"],
        },
    },
    {
        "name": "all_off",
        "description": "Выключает весь свет и закрывает все жалюзи в доме.",
        "input_schema": {
            "type": "object",
            "properties": {},
            "required": [],
        },
    },
]

# ── Tool executor ─────────────────────────────────────────────────────────────

def execute_tool(name: str, inputs: dict, home: HomeManager) -> str:
    try:
        if name == "list_devices":
            state = home.get_all_devices()
            lines = ["=== Освещение ==="]
            for light in state["lights"]:
                status = "ВКЛ" if light["on"] else "ВЫКЛ"
                lines.append(
                    f"[{light['id']}] {light['room']} / {light['name']}: "
                    f"{status}, яркость {light['brightness']}%, цвет {light['color']}"
                )
            lines.append("\n=== Жалюзи ===")
            for blind in state["blinds"]:
                lines.append(
                    f"[{blind['id']}] {blind['room']} / {blind['name']}: "
                    f"открыты на {blind['position']}%"
                )
            return "\n".join(lines)

        elif name == "set_light":
            result = home.set_light(
                light_id=inputs["light_id"],
                on=inputs.get("on"),
                brightness=inputs.get("brightness"),
                color=inputs.get("color"),
            )
            status = "ВКЛ" if result["on"] else "ВЫКЛ"
            return (
                f"✓ {result['room']} / {result['name']}: "
                f"{status}, яркость {result['brightness']}%, цвет {result['color']}"
            )

        elif name == "set_room_lights":
            results = home.set_room_lights(
                room=inputs["room"],
                on=inputs.get("on"),
                brightness=inputs.get("brightness"),
                color=inputs.get("color"),
            )
            lines = [f"✓ Обновлено {len(results)} светильников в «{inputs['room']}»:"]
            for r in results:
                status = "ВКЛ" if r["on"] else "ВЫКЛ"
                lines.append(f"  • {r['name']}: {status}, яркость {r['brightness']}%")
            return "\n".join(lines)

        elif name == "set_blind":
            result = home.set_blind(blind_id=inputs["blind_id"], position=inputs["position"])
            return (
                f"✓ {result['room']} / {result['name']}: открыты на {result['position']}%"
            )

        elif name == "set_room_blinds":
            results = home.set_room_blinds(room=inputs["room"], position=inputs["position"])
            lines = [f"✓ Жалюзи в «{inputs['room']}» открыты на {inputs['position']}%:"]
            for r in results:
                lines.append(f"  • {r['name']}")
            return "\n".join(lines)

        elif name == "all_off":
            result = home.all_off()
            return f"✓ {result['message']}"

        else:
            return f"Неизвестный инструмент: {name}"

    except ValueError as e:
        return f"Ошибка: {e}"
    except Exception as e:
        return f"Системная ошибка: {e}"
