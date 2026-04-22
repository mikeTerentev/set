"""
Magiesta Claude — умный дом через Claude AI.
Запуск: python main.py
Выход:  /выход  или  Ctrl+C
"""

import os
import sys
import anthropic
from devices import HomeManager
from tools import TOOLS, execute_tool

MODEL = "claude-opus-4-7"

SYSTEM_PROMPT = """\
Ты — умный ассистент системы Magiesta Claude, управляющий умным домом.
В твоём доме есть комнаты: Гостиная, Спальня, Кухня, Коридор.
Устройства: светильники (люстры, торшеры, лампы) и жалюзи.

Правила:
• Используй инструменты для любых действий с устройствами.
• Перед ответом всегда вызывай list_devices, если пользователь спрашивает о текущем состоянии.
• Отвечай кратко и по-русски.
• Яркость 0–100, позиция жалюзи 0 (закрыты) – 100 (открыты).
• Цвет в формате HEX (#FFFFFF = белый, #FFF5E0 = тёплый, #FF5500 = оранжевый и т.д.).
• Если пользователь говорит «утро», «вечер», «ночь», «романтика» и т.п. — подбери подходящие настройки сам.
"""


def print_separator():
    print("─" * 60)


def chat():
    client = anthropic.Anthropic()
    home = HomeManager()
    messages: list[dict] = []

    print("╔══════════════════════════════════════════╗")
    print("║        Magiesta Claude — Умный дом       ║")
    print("╠══════════════════════════════════════════╣")
    print("║  Управляйте светом и жалюзи голосом.     ║")
    print("║  Введите /выход или нажмите Ctrl+C       ║")
    print("╚══════════════════════════════════════════╝")
    print()

    while True:
        # ── User input ────────────────────────────────────────────────────────
        try:
            user_input = input("Вы: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nДо свидания!")
            break

        if not user_input:
            continue
        if user_input.lower() in ("/выход", "/exit", "выход", "exit", "quit"):
            print("До свидания!")
            break

        messages.append({"role": "user", "content": user_input})

        # ── Agentic tool-use loop ─────────────────────────────────────────────
        while True:
            response = client.messages.create(
                model=MODEL,
                max_tokens=4096,
                system=[
                    {
                        "type": "text",
                        "text": SYSTEM_PROMPT,
                        "cache_control": {"type": "ephemeral"},
                    }
                ],
                tools=TOOLS,
                messages=messages,
            )

            # Collect text and tool_use blocks
            tool_use_blocks = []
            text_parts = []
            for block in response.content:
                if block.type == "text":
                    text_parts.append(block.text)
                elif block.type == "tool_use":
                    tool_use_blocks.append(block)

            # Append assistant turn to history
            messages.append({"role": "assistant", "content": response.content})

            # If Claude is done (no tool calls), print final answer and break
            if response.stop_reason == "end_turn" or not tool_use_blocks:
                if text_parts:
                    print(f"\nMagiesta: {''.join(text_parts)}\n")
                print_separator()
                break

            # Execute tools and collect results
            tool_results = []
            for tb in tool_use_blocks:
                tool_output = execute_tool(tb.name, tb.input, home)
                tool_results.append(
                    {
                        "type": "tool_result",
                        "tool_use_id": tb.id,
                        "content": tool_output,
                    }
                )

            # Feed results back to Claude
            messages.append({"role": "user", "content": tool_results})


if __name__ == "__main__":
    if not os.environ.get("ANTHROPIC_API_KEY"):
        print("Ошибка: переменная окружения ANTHROPIC_API_KEY не задана.")
        print("Установите её командой: export ANTHROPIC_API_KEY=your-key")
        sys.exit(1)
    chat()
