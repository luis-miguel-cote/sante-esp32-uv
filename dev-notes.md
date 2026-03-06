# Notas de Desarrollo – ESP32 (PlatformIO)

Este archivo contiene comandos útiles para trabajar con el proyecto ESP32 durante el desarrollo.

---

# Compilar el proyecto

Compila el firmware sin subirlo al ESP32.

```
pio run
```

---

# Compilar y subir al ESP32

Compila el proyecto y lo carga al dispositivo.

```
pio run -t upload
```

---

# Abrir monitor serial

Permite ver los logs del ESP32.

```
pio device monitor
```

---

# Subir firmware y abrir monitor automáticamente



pio run -t upload --disable-auto-clean
pio run -t upload --disable-auto-clean

Comando más usado durante desarrollo.

```
pio run -t upload && pio device monitor
```

---

# Limpiar compilación

Elimina archivos compilados. Útil cuando hay errores extraños.

```
pio run -t clean
```

---

# Ver puertos disponibles

Muestra los puertos donde puede estar conectado el ESP32.

```
pio device list
```

---

# Monitor serial con velocidad específica

```
pio device monitor -b 115200
```

---

# Ver configuración del proyecto

Muestra la configuración del archivo `platformio.ini`.

```
pio project config
```

---

# Ver librerías instaladas

Lista todas las librerías usadas en el proyecto.

```
pio lib list
```

---

# Flujo típico de trabajo

Normalmente durante desarrollo:

1. Subir firmware

```
pio run -t upload
```

2. Abrir monitor serial

```
pio device monitor
```

O todo junto:

```
pio run -t upload && pio device monitor
```

---

# Salir del monitor serial

Presionar:

```
CTRL + ]
```

---

# Notas importantes

- Si el ESP32 no conecta, verificar puerto con `pio device list`
- Si hay errores de compilación extraños, ejecutar `pio run -t clean`
- Velocidad del monitor serial recomendada: `115200`