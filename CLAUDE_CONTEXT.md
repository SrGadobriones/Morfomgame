# morfomgame — Contexto para Claude Code

> Este documento resume el estado del proyecto y la configuración MCP para retomar el trabajo en conversaciones futuras. **Léeme al empezar cualquier nueva sesión.**

---

## Proyecto

- **Nombre**: morfomgame
- **Tipo**: RTS estilo Warcraft 3 / AoE
- **Engine**: Unreal Engine **5.6**
- **Path proyecto**: `C:\Users\SrGado\Documents\Unreal Projects\morfomgame`
- **C++ project**: sí (tiene `.sln`, usa módulo `morfomgame`)
- **Lenguaje de gameplay**: Blueprints
- **Input**: Enhanced Input (IMC_RTS + 3 Input Actions)
- **Resolución target**: 1920×1080

## MCP Setup (IMPORTANTE — léelo antes de tocar)

### Plugin activo
**flopperam/unreal-engine-mcp** (MIT) — clonado en `C:/tmp/flopperam-mcp/`, con **parches custom** que añaden:

1. Soporte de clases externas en `CallFunction` nodes (`target_class` param) — **UtilityNodes.cpp**
2. Soporte de clases externas en `VariableGet`/`VariableSet` (`variable_class` param) — **DataNodes.cpp**
3. Comando `execute_python` que ejecuta código Python arbitrario en UE vía `IPythonScriptPlugin` — **EpicUnrealMCPBridge.cpp** + dep `PythonScriptPlugin` en Build.cs

El plugin vive en `Plugins/UnrealMCP/` del proyecto. Si se clona flopperam limpio, re-aplicar los patches.

### Config MCP
**`.mcp.json`** (en raíz del proyecto):
```json
{
  "mcpServers": {
    "unreal": {
      "command": "uv",
      "args": ["--directory", "C:/tmp/flopperam-mcp/Python", "run", "unreal_mcp_server_advanced.py"]
    }
  }
}
```

### Live Coding
- Activado. Atajo custom: **`Ctrl+Shift+F11`** (o el que tengas)
- Para cambios .cpp → Live Coding basta
- Para cambios en Build.cs, headers nuevos, UFUNCTION/UPROPERTY nuevos → **cerrar UE, rebuild externo con UBT, reabrir**

### Rebuild externo (CLI)
```bash
UEROOT="/c/Program Files/Epic Games/UE_5.6"
"$UEROOT/Engine/Binaries/ThirdParty/DotNet/8.0.300/win-x64/dotnet.exe" \
  "$UEROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll" \
  morfomgameEditor Win64 Development \
  -Project="C:/Users/SrGado/Documents/Unreal Projects/morfomgame/morfomgame.uproject" \
  -WaitMutex
```

### TCP directo (bypass MCP Python wrappers)
Plugin escucha en `127.0.0.1:55557`. Protocolo JSON:
```json
{"type":"<command_name>","params":{...}}
```
Comandos útiles: `execute_python`, `read_blueprint_content`, `analyze_blueprint_graph`, `add_blueprint_node`, `connect_nodes`, `set_node_property`, `delete_node`, `compile_blueprint`.

### Notas de fricción conocidas
- Blueprints se buscan con path completo: `/Game/RTS/Blueprints/BP_Unit`, no solo nombre corto.
- `remove_unused_variables` borra TODAS las variables sin ref, no solo las que acabas de reemplazar → cuidado, re-agregar después.
- Trailing spaces en nombres de variables causan bugs silenciosos. Solución: `replace_variable_references` + agregar nuevo limpio + `remove_unused_variables`.
- `CallFunction` con `target_class` requiere el short name (`SpringArmComponent`) o full path (`/Script/Engine.SpringArmComponent`). Para properties use `VariableGet`/`VariableSet` con `variable_class`, no `CallFunction`.

---

## Estado actual del proyecto

### ✅ HUD — `/Content/ui/WBP_MainHUD`

HUD estilo WC3 completado: barra superior oscura con gold/wood/food, minimapa (bottom-left), unit info panel (bottom-center) con HP bar verde, command card 4×3 (bottom-right). Bordes dorados de acento. Paleta tierra/dorada.

- Widget reutilizable: `WBP_CommandButton`
- Widgets clave: `TXT_BuildingName` (mantiene compat con graph existente), `ProgressBarVida` (HP bar), `Btn_Cmd_0_0` a `Btn_Cmd_2_3`, `Btn_Menu`, `Btn_UnitPortrait`, bordes `Border_*`

**Pendiente en HUD**: bindings a datos reales del PlayerController (recursos, unidad seleccionada). Layout listo.

### ✅ BP_RTS_CameraPawn — `/Content/RTS/Player/BP_RTS_CameraPawn`

Cámara isométrica RTS con SpringArm (-60° pitch).

**Variables** (todas Instance Editable):
| Var | Default | Uso |
|---|---|---|
| MoveSpeed | 1500 | WASD pan |
| ZoomSpeed | 200 | Mouse wheel |
| MinZoom / MaxZoom | 600 / 3500 | Clamp del SpringArm.TargetArmLength |
| RotationSpeed | 120 | Q/E rotation (no wired aún) |
| EdgeScrollSpeed / Threshold | 1200 / 15 | Edge scroll (no wired aún) |
| MapBoundsMin / Max | (-10000,-10000,0) / (10000,10000,0) | Clamp de posición (no wired aún) |
| MoveDirection | Vector (-) | Ya existía |

**Grafo zoom ARREGLADO**: ahora usa `SpringArm.TargetArmLength` con Clamp(MinZoom, MaxZoom) en vez del bug anterior de `Add Actor World Offset` en Z.

**Pendiente**: Q/E rotation, edge scroll, clamp a MapBounds.

### ✅ Enhanced Input — `/Content/RTS/Player/`

- `IA_Move` (Axis2D): WASD con SwizzleAxis + Negate modifiers correctos
- `IA_Zoom` (Axis1D): MouseScrollUp (+1), MouseScrollDown + Negate (−1)
- `IA_SelectUnit` (Boolean): LeftMouseButton
- `IMC_RTS`: las 7 mappings bien configuradas

**Todo correcto, no tocar.**

### ✅ Jerarquía de BPs — `/Content/RTS/Blueprints/`

```
Actor
└── BP_RTSActor  (base común, 6 vars, 6 funciones)
    ├── BP_Building  (+4 vars: ProductionTime, UnitToSpawn, ResourceCost, SpawnOffset)
    └── BP_Unit  (+9 vars de combate/movimiento)
        └── BP_CombatUnit  ← REPARENTADO (antes Actor, ahora BP_Unit)
```

**BP_RTSActor** (variables + defaults):
- TeamID=0, DisplayName, MaxHP=100, CurrentHP=100, Armor=0, **bIsAlive=True**
- Funciones: SetSelected, GetDisplayName, Die, ReceiveDamage, GetCurrentHP, GetMaxHP

**BP_Unit** (defaults):
- Level=1, MaxLevel=10, AttackSpeed=1.0, AttackRange=150, AttackDamage=10, AttackCooldown=0
- MoveSpeed=300, RotationSpeed=360, CurrentTarget (object ref)

**BP_Building** (defaults):
- ProductionTime=5, UnitToSpawn (class ref, no seteado), ResourceCost=50, SpawnOffset=(300,0,0)
- Override de MaxHP=500, CurrentHP=500

**BP_CombatUnit**: sin vars propias, hereda todo de BP_Unit.

### ✅ BP_RTS_GameMode / PlayerController

- GameMode tiene `DefaultPawnClass = BP_RTS_CameraPawn_C`, `PlayerControllerClass = BP_RTS_PlayerController_C`
- PlayerController hace: `AddMappingContext(IMC_RTS)`, `bShowMouseCursor=true`, raycast bajo cursor en IA_SelectUnit, cast a BP_RTSActor, setea `SelectedUnit`
- HUD Class sigue siendo `/Script/Engine.HUD` (default) — NO referencia el WBP_MainHUD todavía

---

## Pendientes (por prioridad)

### 🔴 Crítico para gameplay mínimo
1. **Componentes visuales** en BP_Building y BP_Unit (StaticMesh + Collision). Sin mesh no se ven.
2. **Instanciar WBP_MainHUD** desde BP_RTS_PlayerController en BeginPlay y guardarlo en `HUD_Ref`.
3. **Bindings del HUD** a variables del PlayerController (recursos, unidad seleccionada).

### 🟡 Importantes
4. **Lógica de ataque en Tick** de BP_Unit: si hay CurrentTarget válido y en rango → restar cooldown, disparar `ReceiveDamage` del target cuando cooldown<=0.
5. **ProduceUnit en BP_Building**: parametrizar con `UnitToSpawn` en vez de hardcode a BP_Unit.
6. **Movimiento de unidades**: BP_Unit es Actor, no Pawn/Character — evaluar si convertir a Character (mejor con NavMesh y AI) o dejar como Actor con MoveTo custom.

### 🟢 Polish
7. **Q/E rotación** en BP_RTS_CameraPawn (usar RotationSpeed).
8. **Edge scroll** en BP_RTS_CameraPawn (usar EdgeScrollSpeed/Threshold).
9. **Clamp de posición** con MapBoundsMin/Max.
10. **Team colors** aplicando materiales por TeamID.
11. **Barra de selección** (box select múltiples unidades).
12. **Minimapa real**: SceneCapture2D + Render Target apuntando al suelo.

---

## Paths clave

| Tipo | Path |
|---|---|
| Plugin MCP | `Plugins/UnrealMCP/Source/UnrealMCP/` |
| Scripts Python MCP | `C:/tmp/flopperam-mcp/Python/` |
| Widget HUD | `/Content/ui/WBP_MainHUD.uasset` |
| BP_CommandButton | `/Content/ui/WBP_CommandButton.uasset` |
| BPs RTS | `/Content/RTS/Blueprints/` |
| BP CameraPawn | `/Content/RTS/Player/BP_RTS_CameraPawn.uasset` |
| Input Actions + IMC | `/Content/RTS/Player/IA_*.uasset, IMC_RTS.uasset` |

## Comandos MCP útiles (cheat sheet)

### Vía MCP normal (desde Claude)
- `mcp__unreal__add_blueprint_node` con `node_type` (VariableGet/VariableSet/CallFunction/Branch/etc.)
- `mcp__unreal__connect_nodes` — wire pins
- `mcp__unreal__delete_node`
- `mcp__unreal__set_node_property` — pos_x/pos_y o `action=add_pin`
- `mcp__unreal__compile_blueprint`
- `mcp__unreal__read_blueprint_content` / `analyze_blueprint_graph`
- `mcp__unreal__create_variable` / `set_blueprint_variable_properties`
- `mcp__unreal__execute_python` ← **el más poderoso**, ejecuta cualquier API Python de UE

### Python dentro de UE (via execute_python)
```python
import unreal
lib = unreal.BlueprintEditorLibrary
bp = unreal.EditorAssetLibrary.load_asset("/Game/...")
gen = lib.generated_class(bp)
cdo = unreal.get_default_object(gen)
cdo.set_editor_property("VarName", value)
lib.compile_blueprint(bp)
unreal.EditorAssetLibrary.save_asset(bp.get_path_name())
```

### TCP directo (cuando MCP falla o es lento)
```python
import socket, json
s = socket.socket(); s.settimeout(60); s.connect(('127.0.0.1', 55557))
s.sendall(json.dumps({'type': 'execute_python', 'params': {'code': '...'}}).encode())
# read until complete JSON
```

---

## Cómo retomar (para futuro Claude)

1. Lee este archivo primero (`CLAUDE_CONTEXT.md`).
2. Verifica MCP: pide `/mcp` al usuario — debe decir `unreal · connected` con 43+ tools.
3. Si el MCP falla: revisa `.mcp.json`, pide al usuario que mate procesos node/uv zombies, reinicie Claude Code.
4. Si UE se abre pero el plugin no compila: rebuild externo con el comando en la sección "Rebuild externo".
5. Si el usuario pregunta por el estado → apunta a la sección "Estado actual del proyecto".
6. Si el usuario pide una feature nueva → consulta "Pendientes" primero.

**Principio guía**: minimiza rebuild cycles. Preferir Live Coding > rebuild externo > close/reopen UE. Preferir Python API vía `execute_python` > parches C++ nuevos.
