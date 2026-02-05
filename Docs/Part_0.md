# Part 0: Introduction + Mental Model

The following documents in this folder will outline the process of how to transition from an IDE-managed build, like STM32CubeIDE, to a Makefile-based workflow on a Windows 11 device. Don’t concern yourself too much if you don’t know what these terms mean right now. Think of it as essentially you are still using the same underlying tools in the STM32CubeIDE, but now, the IDE doesn’t manage them for you.

### Pre-requisites:
Make sure that you have STM32CubeIDE installed, a STM32G0xx series board, Git, and the text editor of your choice, this tutorial will be using VSCode. Additionally, fundamental knowledge of file explorer would be beneficial to your understanding of the instructions present in this document. The following documents will assume you have basic knowledge of STM32CubeIDE and have programmed a STM32 via this IDE in the past.

### Mental Model of theCubeIDE to Toolchain Transition:
Functionally, the resulting firmware is the same between using STM32CubeIDE and a custom toolchain environment. It’s primarily transitioning from a GUI-based workflow to a text + command-driven workflow. It is not transitioning from “safe” to “unsafe” code, or from a beginner to an expert. There are many parallels between STM32CubeIDE to the custom toolchain environment, see the table below for reference.

| CubeIDE Concept | Toolchain Equivalent | What actually changes |
| ------------- | -------------------- | -------------------- |
| New STM32 Project | Git Repository + Makefile | Project definition moves to text |
| HAL/Generated code | Direct use of CMSIS and device headers (HAL optional) | Less abstraction from concepts, more control |
| Build button | make | Same compiler, more explicit |
| Flash button | make flash (OpenOCD) | Same ST-LINK scripted |
| IDE Terminal | System Terminal | Nothing |

The **key difference** is that the project configuration moves from IDE-generated metadata into a human-readable Makefile.
