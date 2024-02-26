# Mini 3D Engine Project
## Practical Assignment CG - 2023/2024

This project aims to develop a mini scene graph-based 3D engine for handling graphical primitives and hierarchical scenes. The assignment is divided into four phases, each building upon the previous one, and the goal is to create a functional 3D engine with advanced features.

## Phase 1 – Graphical Primitives

### Generator
- The generator application creates model files for graphical primitives such as Plane, Box, Sphere, and Cone.
- Example commands:
  - `generator sphere 1 10 10 sphere.3d`
  - `generator box 2 3 box.3d`
  - `generator cone 1 2 4 3 cone.3d`
  - `generator plane 1 3 plane.3d`

### Engine
- Reads XML configuration file.
- Displays models based on the configuration.
- Example XML configuration:
```xml
<world>
  <window width="512" height="512" />
  <camera>
    <!-- Camera settings -->
  </camera>
  <group>
    <models>
      <model file="plane.3d" />
      <model file="cone.3d" />
    </models>
  </group>
</world>

