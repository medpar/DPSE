// Diseño de caja para videoconsola portátil
// Autor: Jesús Manuel Hernández Mangas (código original)
// Modificado y completado para carcasa exterior imprimible en 3D
// Fecha: 2023/03/29 (original), modificado en 2023

// Parámetros globales
$fn = 30;           // Resolución de las curvas
radio = 2.5;        // Radio para redondear bordes
altura = 20 - 2 * radio; // Altura total de la carcasa
dh = 5;             // Desplazamiento en altura para partes hundidas

// Módulo E(): Forma básica con esquinas redondeadas
module E() {
    minkowski() {
        sphere(2 * radio);
        cylinder(altura, r = radio);
    }
}

// Módulo B(): Forma básica más baja
module B() {
    minkowski() {
        sphere(2 * radio);
        cylinder(altura - 2 * dh, r = radio);
    }
}

// Módulo CAJA(): Forma básica de la carcasa
module CAJA() {
    union() {
        // Lado izquierdo hundido
        hull() {
            translate([0 + 2 * radio, 14 + 2 * radio, dh]) B();
            translate([8 + 2 * radio, 90 - 2 * radio, dh]) B();
            translate([19 + 2 * radio, 94 - 2 * radio, 0]) E();
            translate([41 + 2 * radio, 90 - 2 * radio, 0]) E();
            translate([29 + 2 * radio, 2 + 2 * radio, 0]) E();
            translate([98.5, 11 + 2 * radio, 0]) E();
        }
        // Lado izquierdo plano
        hull() {
            translate([41 + 2 * radio, 90 - 2 * radio, 0]) E();
            translate([98.5, 90 - 2 * radio, 0]) E();
            translate([98.5, 11 + 2 * radio, 0]) E();
        }
        // Lado derecho plano
        hull() {
            translate([154 - 2 * radio, 90 - 2 * radio, 0]) E();
            translate([98.5, 90 - 2 * radio, 0]) E();
            translate([98.5, 11 + 2 * radio, 0]) E();
        }
        // Lado derecho hundido
        hull() {
            translate([154 - 2 * radio, 90 - 2 * radio, 0]) E();
            translate([176 - 2 * radio, 94 - 2 * radio, 0]) E();
            translate([187 - 2 * radio, 90 - 2 * radio, dh]) B();
            translate([197 - 2 * radio, 14 + 2 * radio, dh]) B();
            translate([168 - 2 * radio, 2 + 2 * radio, 0]) E();
            translate([98.5, 11 + 2 * radio, 0]) E();
        }
    }
}

// Módulo CAJA_SKIN(): Paredes externas de la carcasa
module CAJA_SKIN() {
    translate([0, 0, 2 * radio])
    difference() {
        CAJA();
        translate([5, 2, 1]) scale([0.96, 0.96, 0.90]) CAJA(); // Paredes de grosor uniforme
    }
}

// Módulo BOX_BOT(): Parte inferior de la carcasa
module BOX_BOT() {
    difference() {
        CAJA_SKIN();
        translate([-10, -10, altura / 2 + 2 * radio]) cube([250, 150, altura], false);
    }
}

// Módulo BOX_TOP(): Parte superior de la carcasa
module BOX_TOP() {
    difference() {
        CAJA_SKIN();
        translate([-10, -10, -altura / 2 + 2 * radio]) cube([250, 150, altura - 2], false);
    }
}

// Módulo Vaciado(): Espacios para componentes
module Vaciado() {
    x0 = 87.5; y0 = 27.0;
    h = altura / 2;
    union() {
        // Pantalla TFT 3.98" NT35510
        translate([45.5 + 2.5 + 8, 19 + 5 + 2, h]) cube([90, 53, h + 11]);
        // Botones de acción (izquierda)
        translate([171 - 147.5, 60.5 + 5, h]) cylinder(h = h + 11, r = 5.7);
        translate([164.2 - 147.5, 50.1 + 5, h]) cylinder(h = h + 11, r = 5.7);
        translate([177.9 - 147.5, 50.1 + 5, h]) cylinder(h = h + 11, r = 5.7);
        translate([171 - 147.5, 39.7 + 5, h]) cylinder(h = h + 11, r = 5.7);
        // Botones de acción (derecha)
        translate([171, 60.5 + 5, h]) cylinder(h = h + 11, r = 5.7);
        translate([164.2, 50.1 + 5, h]) cylinder(h = h + 11, r = 5.7);
        translate([177.9, 50.1 + 5, h]) cylinder(h = h + 11, r = 5.7);
        translate([171, 39.7 + 5, h]) cylinder(h = h + 11, r = 5.7);
        // Slot Switch encendido
        translate([96.5, 84.0, 7.5]) cube([14, 10, 7.0 + 1]);
        // Slot USB mini B
        translate([115.5, 84.0, 5.5]) cube([14, 10, 7.5]);
        // Slot potenciómetro de audio
        translate([129.0, 84.0, 4.0]) cube([18, 10, 5.0]);
        // Hueco PCB1
        translate([83.5, 23.5, 6.0]) cube([102, 65, 9]);
        // Agujeros tornillos PCB1
        translate([87.5, 27, 2]) cylinder(h = 15, r = 3);
        translate([87.5, 85.5, 2]) cylinder(h = 15, r = 3);
        // Hueco PCB2
        translate([8.1, 35.4, 7.0]) cube([75.4, 43, 6]);
        // Agujero tornillo PCB2
        translate([81.5, 72.0, 2]) cylinder(h = 15, r = 3);
        // Agujeros tornillos TFT 3.98" NT35510
        translate([41.4 + 3, 85.0 - 7.0, altura + 2 * radio]) cylinder(h = 1, r = 3);
        translate([41.4 + 3, 17.3 + 7.5 + 8, altura + 2 * radio]) cylinder(h = 1, r = 3);
        translate([155.2 - 5, 85.0 - 7.5 + 8, altura + 2 * radio]) cylinder(h = 1, r = 3);
        translate([155.2 - 5, 17.3 + 7.0, altura + 2 * radio]) cylinder(h = 1, r = 3);
        // Slots altavoces
        SpeakerNeg(14, 12.0, 0, 8, 0.8);  // Izquierda
        SpeakerNeg(161, 12.0, 0, 8, 0.8); // Derecha
        // Slot microSD
        translate([132, 0, 3]) cube([19, 10, 7]);
    }
}

// Módulo Tornillo(): Soporte para tornillos
module Tornillo(a) {
    difference() {
        cylinder(h = a, r = 3);
        cylinder(h = a, r = 1.5);
    }
}

// Módulo SpeakerNeg(): Agujeros para altavoces
module SpeakerNeg(X, Y, Z, a, b) {
    h = altura + 2 * radio - 2;
    for (i = [0:1:4])
        for (j = [0:1:4]) {
            if ((i - 2) * (i - 2) + (j - 2) * (j - 2) < 2.5 * 2.5)
                translate([X + i * 5, Y + j * 5, Z + h]) cylinder(h = a, r = b);
        }
    translate([X + 2 * 5, Y + 2 * 5, Z + h - 13]) cylinder(h = a + 8, r = 14 + 1);
}

// Módulo SpeakerPos(): Soporte para altavoces
module SpeakerPos(X, Y, Z, a, b) {
    h = altura + 2 * radio - 2;
    intersection() {
        translate([0, 0, 2 * radio]) CAJA();
        translate([X + 2 * 5, Y + 2 * 5, Z + h + 1.5])
        difference() {
            cylinder(h = a + 16, r = 16 + 1);
            cylinder(h = a + 16, r = 14 + 1);
        }
    }
}

// Módulo SumarBloquesTop(): Elementos adicionales en TOP
module SumarBloquesTop() {
    // Tornillos TFT 3.98" NT35510
    translate([41.4 + 3, 85.0 - 7.0, altura + 2 * radio - 0.3]) Tornillo(5 - 0.3);
    translate([41.4 + 3, 17.3 + 7.5, altura + 2 * radio - 0.3]) Tornillo(5 - 0.3);
    translate([155.2 - 5, 85.0 - 7.0, altura + 2 * radio - 0.3]) Tornillo(5 - 0.3);
    translate([155.2 - 5, 17.3 + 7.5, altura + 2 * radio - 0.3]) Tornillo(5 - 0.3);
    // Soportes altavoces
    SpeakerPos(14, 12.0, 0, 8, 0.8);  // Izquierda
    SpeakerPos(161, 12.0, 0, 8, 0.8); // Derecha
}

// Módulo SumarBloquesBot(): Elementos adicionales en BOTTOM
module SumarBloquesBot() {
    x0 = 87.5; y0 = 27.0;
    // Soportes PCB1
    translate([x0, y0, 0]) {
        dx = 94.0; dy = 58.5; h = 5.6;
        translate([0, 0, 1.0]) Tornillo(h);        // (0,0)
        translate([0, dy, 1.0]) Tornillo(h);       // (0,1)
        translate([dx, 0, 2.5]) cylinder(h = h + 3 - 1.5, r = 1.5);  // (1,0) Pitorro
        translate([dx, dy, 3.3]) cylinder(h = h + 3 - 2.3, r = 1.5); // (1,1) Pitorro
    }
    // Soportes PCB2
    translate([x0 - 75, y0 + 15, 0]) {
        dx = 69.5; dy = 30.0; h = 5.6;
        translate([0, 0, 2.6]) cylinder(h = h + 3 - 1.6, r = 1.5);  // (0,0) Pitorro
        translate([0, dy, 3.3]) cylinder(h = h + 3 - 2.3, r = 1.5); // (0,1) Pitorro
        translate([dx, dy, 1.0]) Tornillo(h);       // (1,1)
    }
    // Soportes TFT 3.98" NT35510
    translate([x0 - 46.0 + 2.5, y0 - 9.7 + 8, 0]) {
        h = 20;
        translate([0, 0, 1.0]) Tornillo(h);        // (0,0)
        translate([0, 53, 1.0]) Tornillo(h);       // (0,1)
    }
}

// Módulo TOP(): Parte superior de la carcasa
module TOP() {
    intersection() {
        translate([0, 0, 2 * radio]) CAJA();
        union() {
            difference() {
                BOX_TOP();
                Vaciado();
            }
            SumarBloquesTop();
        }
    }
}

// Módulo BOTTOM(): Parte inferior de la carcasa
module BOTTOM() {
    intersection() {
        translate([0, 0, 2 * radio]) CAJA();
        union() {
            difference() {
                BOX_BOT();
                Vaciado();
            }
            SumarBloquesBot();
        }
    }
}

// Renderizado de las partes para impresión 3D
// Descomenta una línea a la vez para generar cada parte por separado
TOP();      // Parte superior
BOTTOM(); // Parte inferior