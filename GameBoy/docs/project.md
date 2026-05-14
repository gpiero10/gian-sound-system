# 🕹️ Game Boy Emulator – Estructura y Plan de Desarrollo

## 🧠 1. Idea general

Un emulador de Game Boy no es un solo programa, sino un conjunto de componentes que interactúan:

```
CPU ↔ MEMORIA ↔ CARTUCHO
          ↕
        PPU (video)
          ↕
        INPUT
          ↕
        TIMER
```

👉 El objetivo es replicar esta arquitectura en código.

---

## 📦 2. Estructura del proyecto

```
GameBoy/
├── src/
│   ├── main.c
│   ├── cpu.c
│   ├── memory.c
│   ├── cartridge.c
│   ├── ppu.c
│   ├── timer.c
│   ├── input.c
│
├── include/
│   ├── cpu.h
│   ├── memory.h
│   ├── cartridge.h
│   ├── ppu.h
│   ├── timer.h
│   ├── input.h
│
├── rom/
│   └── juegos.gb
│
├── docs/
│   └── diseño.md
```

```
CPU
 ↓
BUS  ← (decide a dónde ir)
 ↓
 ├── Cartridge
 ├── PPU (VRAM + OAM + restricciones)
 ├── WRAM
 ├── IO
 └── HRAM
```
---

## 🔥 3. Orden recomendado de implementación

### 🥇 Paso 1: Cartridge

Archivo: `cartridge.c`

Responsabilidades:

* cargar ROM desde archivo
* parsear header
* (más adelante) manejar MBC (banking)

---

### 🥈 Paso 2: Memory (CRÍTICO)

Archivo: `memory.c`

Funciones base:

```c
uint8_t read(uint16_t addr);
void write(uint16_t addr, uint8_t val);
```

Responsabilidad:

* mapear TODO el sistema:

  * ROM
  * RAM
  * registros de hardware (0xFF00–0xFF7F)

👉 Este módulo es el corazón del emulador.

---

### 🥉 Paso 3: CPU

Archivo: `cpu.c`

Responsabilidades:

* registros (AF, BC, DE, HL, PC, SP)
* ciclo fetch-decode-execute

Empezar por:

* NOP
* LD
* JP

---

### 🏅 Paso 4: Timer

Archivo: `timer.c`

* manejar registros como `0xFF04`
* incrementar según ciclos de CPU

---

### 🎮 Paso 5: Input

Archivo: `input.c`

* leer teclado
* mapear botones a `0xFF00`

---

### 🎨 Paso 6: PPU (video)

Archivo: `ppu.c`

* renderizado de pantalla
* tiles
* scanlines

⚠️ Es la parte más compleja → dejar para el final

---

## 🧠 4. Loop principal

```c
while (running) {
    cycles = cpu_step();
    timer_update(cycles);
    ppu_update(cycles);
}
```

👉 Todo se sincroniza en base a ciclos.

---

## 🗺️ 5. Mapa de memoria (base)

```
0x0000–0x7FFF → ROM
0x8000–0x9FFF → VRAM
0xA000–0xBFFF → External RAM
0xC000–0xDFFF → Work RAM
0xFF00–0xFF7F → I/O Registers
```

---

## 📚 6. Uso de Markdown / documentación

Usar archivos `.md` para:

### ✔ Diseño

```
## memory map
## cpu instructions
## TODO
```

---

### ✔ Seguimiento

```
## CPU

- [x] NOP
- [ ] LD
- [ ] ADD
```

---

### ✔ Notas técnicas

```
## Header checksum
## MBC behavior
```

---

## 🧠 7. Estrategia de desarrollo

❌ NO hacer:

* todo junto
* CPU + PPU + memoria mezclados

✔ HACER:

* módulos independientes
* probar cada uno por separado
* luego integrar

---

## 🎯 8. Primer objetivo realista

1. Leer ROM correctamente
2. Parsear header
3. Ejecutar primeras instrucciones
4. No crashear

Después:

* pasar tests de CPU
* implementar más instrucciones
* recién ahí gráficos

---

## 🔥 9. Tips clave

* `memory.c` define todo el sistema
* la CPU depende de `read/write`
* los registros de hardware son direcciones especiales
* cada módulo debe ser independiente

---

## 🚀 10. Filosofía

> Primero hacerlo funcionar
> Después hacerlo correcto
> Después hacerlo rápido

---

## 🧩 Próximos pasos sugeridos

* implementar `memory.c` completo
* crear estructura de CPU básica
* empezar con instrucciones simples
* agregar debug/logs

---
