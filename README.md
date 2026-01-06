# 🍋‍🟩 SynpaDS: open-source Nintendo DS emulator
<img width="1920" height="700" alt="BannerPNG" src="https://github.com/user-attachments/assets/8df4b823-c0c7-4006-9bb1-3bb94dc21242" />

**Synpad** is an experimental, open-source emulator for the Nintendo DS, written in C++. Its main goal is to provide a modular, extensible foundation for emulating the ARM9 CPU, memory, and GPU of the DS while rendering graphics using OpenGL via GLAD and GLFW.

The project is **designed for learning, experimentation, and progressive development, providing developers and enthusiasts with a platform to:**

Understand how the Nintendo DS hardware works, including its **CPU, memory mapping, and graphics pipeline.**

Experiment with *emulator* development techniques, including instruction decoding, memory access, and real-time rendering.

**Build modular systems where the GPU, CPU, and renderer are decoupled for easier maintenance and future feature additions.**

Unlike commercial emulators, SynPad focuses on simplicity and educational value, gradually implementing CPU instructions, VRAM access, sprite rendering, and eventual support for ROM files. This approach allows contributors to implement one subsystem at a time, test it thoroughly, and see immediate visual feedback as pixels are drawn to the screen.

## The emulator currently includes:

- A minimal ARM9 CPU with basic instruction support (MOV, ADD, SUB, CMP, AND, ORR, EOR).

- A memory system capable of reading and writing 32-bit words.

- A simple GPU managing a VRAM buffer (256×192 pixels) and interfacing with a modular renderer.

- An OpenGL renderer that displays VRAM content in a window with scaling and texture mapping.

- The project aims to gradually support more advanced DS features, including tile maps, sprites, palette handling, and ROM execution, while keeping the code modular and accessible to contributors.

> [!NOTE]  
> More features will be implemented in the future.
>
> 
## Contributions
Contributions, suggestions, and bug reports are highly welcome!

1.  **Fork** the project.
2.  Create a **Branch** for your feature (`git checkout -b feature/MyNewFeature`).
3.  **Commit** your changes (`git commit -m 'feat: Add feature X'`).
4.  **Push** to the branch (`git push origin feature/MyNewFeature`).
5.  Open a **Pull Request**.

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for more details.
