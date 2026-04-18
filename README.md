# morfomgame

RTS-style game prototype built with **Unreal Engine 5.6**, inspired by Warcraft 3 / Age of Empires. Gameplay runs on Blueprints; camera, input and scaffolding are wired up. Iteration is driven via a custom MCP (Model Context Protocol) plugin that lets AI agents inspect and edit Blueprints, widgets and assets programmatically.

---

## Requirements

- **Unreal Engine 5.6** (Epic Games Launcher)
- **Visual Studio 2022** with *Game development with C++* workload (to compile the C++ module + MCP plugin)
- **Git** with **Git LFS** (`git lfs install` once per machine)
- **Python 3.10+** (for MCP server helpers)
- **uv** — fast Python installer: https://github.com/astral-sh/uv
- **Node.js** (optional, only if using an MCP server that ships as an npm package)

---

## Clone + first-time setup

```bash
git clone https://github.com/SrGadobriones/Morfomgame.git
cd Morfomgame
git lfs pull          # download LFS-tracked assets (.uasset, .umap, images)
```

### Re-import excluded Marketplace sample packs (optional)

To keep repo size small, these free Epic Marketplace packs are **excluded from git** — re-import them from the Epic Games Launcher → Marketplace if needed:

| Pack | Folder | Size |
|---|---|---|
| Stylized Provencal | `Content/StylizedProvencal/` | ~313 MB |
| Goblin Character Pack | `Content/Goblin/` | ~692 MB |
| UE5 Third Person Template | `Content/Demo/` | ~53 MB |

### Open the project

1. Double-click `morfomgame.uproject`
2. On first open, UE will prompt to rebuild missing modules (the custom `UnrealMCP` C++ plugin) — click **Yes**
3. If the auto-rebuild fails, right-click `morfomgame.uproject` → *Generate Visual Studio project files* → open `morfomgame.sln` in VS 2022 → build with `Development Editor / Win64`

---

## Project structure

```
morfomgame/
├── Content/
│   ├── RTS/
│   │   ├── Blueprints/      # BP_RTSActor, BP_Building, BP_Unit, BP_CombatUnit,
│   │   │                    #   BP_RTS_GameMode, BP_RTS_PlayerController
│   │   └── Player/          # BP_RTS_CameraPawn, IA_Move, IA_Zoom,
│   │                        #   IA_SelectUnit, IMC_RTS
│   ├── ui/                  # WBP_MainHUD, WBP_CommandButton
│   ├── Maps/                # Map_RTS_Test
│   └── Materials/           # M_Selection
├── Source/
│   └── morfomgame/          # C++ game module (minimal)
├── Plugins/
│   └── UnrealMCP/           # Custom MCP plugin (patched flopperam fork)
├── Config/                  # Project config (DefaultEngine.ini, etc.)
└── CLAUDE_CONTEXT.md        # Dev context & state for AI-assisted iteration
```

### Blueprint inheritance

```
Actor
└── BP_RTSActor           [TeamID, DisplayName, MaxHP=100, CurrentHP=100, Armor, bIsAlive]
    ├── BP_Building       [+ProductionTime, UnitToSpawn, ResourceCost, SpawnOffset]
    └── BP_Unit           [+Level, AttackSpeed, AttackRange, AttackDamage,
        │                  AttackCooldown, MoveSpeed, RotationSpeed, CurrentTarget]
        └── BP_CombatUnit [inherits all]
```

### Input

- **IA_Move** (Axis2D) — WASD with SwizzleAxis + Negate modifiers
- **IA_Zoom** (Axis1D) — Mouse wheel up/down
- **IA_SelectUnit** (Boolean) — Left mouse button
- **IMC_RTS** — mapping context applied by `BP_RTS_PlayerController` at BeginPlay

### Camera

`BP_RTS_CameraPawn` uses an isometric SpringArm (−60° pitch). Zoom modifies `SpringArm.TargetArmLength` with `Clamp(MinZoom=600, MaxZoom=3500)` — camera height stays constant, feels natural.

---

## Dev workflow

### Regular git flow

Commits work normally — `.gitattributes` auto-routes `.uasset`, `.umap`, `.fbx`, images, etc. through Git LFS.

```bash
git add .
git commit -m "Your message"
git push
```

### Working with the MCP plugin (AI-assisted dev)

The `Plugins/UnrealMCP/` C++ plugin exposes a TCP server on port **55557** that an MCP client (Claude, Cursor, etc.) uses to read/write Blueprints, widgets and execute arbitrary UE Python API code. See `CLAUDE_CONTEXT.md` for full details:

- Plugin is based on [flopperam/unreal-engine-mcp](https://github.com/flopperam/unreal-engine-mcp) (MIT) with custom patches
- Live Coding (`Ctrl+Shift+F11` in editor) hot-reloads C++ changes for most edits
- Python MCP server lives at `C:/tmp/flopperam-mcp/Python/` (configured via `.mcp.json`)

### Build from CLI

```bash
# Windows (Git Bash)
UEROOT="/c/Program Files/Epic Games/UE_5.6"
"$UEROOT/Engine/Binaries/ThirdParty/DotNet/8.0.300/win-x64/dotnet.exe" \
  "$UEROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll" \
  morfomgameEditor Win64 Development \
  -Project="$(pwd)/morfomgame.uproject" -WaitMutex
```

---

## What's working

- ✅ HUD scaffolding (resources bar, minimap frame, unit info, 4×3 command card, HP bar)
- ✅ Isometric camera with correct zoom (SpringArm length, not Z translation)
- ✅ Enhanced Input fully wired (WASD pan, mouse wheel zoom, LMB select)
- ✅ PlayerController raycasts under cursor, selects RTS actors
- ✅ Base class hierarchy with sensible defaults (HP, combat stats, team ID)

## What's next

See the **Pendientes** section in [`CLAUDE_CONTEXT.md`](CLAUDE_CONTEXT.md) — prioritised backlog includes:

1. Static mesh + collision components on BP_Building / BP_Unit
2. Instantiate `WBP_MainHUD` from the PlayerController; wire bindings
3. Attack/target logic in BP_Unit Tick
4. Q/E camera rotation, edge scroll, map bounds clamp
5. Box-select multiple units, real minimap with SceneCapture2D

---

## License

Project code & assets: **All rights reserved** (personal project, not yet open-licensed). Custom MCP plugin modifications live in `Plugins/UnrealMCP/` and are derived from the MIT-licensed flopperam/unreal-engine-mcp project.
