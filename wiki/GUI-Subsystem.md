# GUI Subsystem

Windowed compositor with 1024×768 32bpp framebuffer, 32 windows, taskbar, Start menu, 4 workspaces.

## VBE framebuffer (`vbe.c`)
Bochs VBE extensions, LFB at 0xE0000000. `fb.c` provides put_pixel, fill_rect, blit, draw_char.

## Bitmap font (`font.c`)
IBM VGA 8×16 ROM font, 256 glyphs.

## Compositor (`compositor.c`)
Windows in z-order, per-workspace, with titlebar drag, resize, minimize, close. Yields CPU when idle (sleep 5 ms).

## Built-in apps
- **Terminal:** 80×24 grid, scrollback, tab-completion, per-shell CWD, command history
- **File Manager:** VFS browsing, copy/paste, drag-drop, context menu, search
- **Text Editor:** File open/save, cursor nav, click-to-position, scroll
- **Image Viewer:** Test pattern, zoom/pan
- **Sound Test:** PC speaker + SB16 buttons
- **Calculator:** Basic arithmetic
- **Task Manager:** Process list
- **DOOM:** VGA mode 13h port (requires doom1.wad)
- **Paint:** Freehand drawing with mouse
