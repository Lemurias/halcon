################################################################################
# ██████████████████████████████████████████████████████████████████████████████
# █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
# █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
# █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
# █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
# █    ███    ██▌          ██▌   ███████▄   ▀██▀   ▐██    ███    ███   ██▄     █
# █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
# ██████████████████████████████████████████████████████████████████████████████
# █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
# ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
################################################################################
# Author: Patricio Reus Merlo
# Date: 06/06/2024
################################################################################
# MIT License
# 
# Copyright (c) 2024 Fundacion Fulgor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

###############################################################################
# PYTHON MODULES
###############################################################################

import os

################################################################################
# HALCON MODULES
################################################################################

from halcon import Beamer, NoEscape

################################################################################
# IMG DIRECTORY
################################################################################

img_dir = "./img"
img_dir = os.path.abspath(img_dir)

################################################################################
# EXAMPLE SLIDES
################################################################################

lorem1 = '''
Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere
cubilia Curae; Phasellus facilisis tortor vel imperdiet vestibulum. Vivamus et
mollis risus. Proin ut enim eu leo volutpat tristique. Vivamus quam enim,
efficitur quis turpis ac, condimentum tincidunt tellus. Praesent non tellus in
quam tempor dignissim. Sed feugiat ante id mauris vehicula, quis elementum nunc
molestie. Pellentesque a vulputate nisi, ut vulputate ex. Morbi erat eros,
aliquam in justo sed, placerat tempor mauris. In vitae velit eu lorem dapibus
consequat. Integer posuere ornare laoreet.
'''

lorem2 = '''
Donec pellentesque libero id tempor aliquam. Maecenas a diam at metus varius
rutrum vel in nisl. Maecenas a est lorem. Vivamus tristique nec eros ac
hendrerit. Vivamus imperdiet justo id lobortis luctus. Sed facilisis ipsum ut
tellus pellentesque tincidunt. Mauris libero lectus, maximus at mattis ut,
venenatis eget diam. Fusce in leo at erat varius laoreet. Mauris non ipsum
pretium, convallis purus vel, pulvinar leo. Aliquam lacinia lorem dapibus
tortor imperdiet, quis consequat diam mollis.
'''

lorem3 = '''
Praesent accumsan ultrices diam a eleifend. Vestibulum ante ipsum primis in
faucibus orci luctus et ultrices posuere cubilia Curae; Suspendisse accumsan
orci ut sodales ullamcorper. Integer bibendum elementum convallis. Praesent
accumsan at leo eget ullamcorper. Maecenas eget tempor enim. Quisque et nisl
eros.
'''

################################################################################
# PREAMBLE
################################################################################

# Create Beamer
doc = Beamer()

# Theme
doc.theme("Madrid", "default")

# Packages
doc.package('lipsum')

# Title
doc.preamble_line_comment()
doc.title("HALCON", "HALCON")
doc.subtitle("DSP Simulation Engine")
doc.author("Patricio Reus Merlo", "P.ReusMerlo")
doc.institute("Fundación Fulgor")
doc.date()

doc.enable_section_frame("Agenda", "Próximo ...")
doc.enable_subsection_frame("Agenda", "Próximo ...")

################################################################################
# TITLE PAGE
################################################################################

with doc.create_frame() as frame:
    frame.title_page()

################################################################################
# TABLE OF CONTENT
################################################################################

with doc.create_frame("Agenda") as frame:
    frame.table_of_contents()

################################################################################
# SECTION
################################################################################

with doc.create_section(title="Texto"):

    ###################################
    # FRAME
    ###################################

    with doc.create_frame(title="Texto",
                          subtitle="Solo texto ...") as frame:
        
        frame.append(NoEscape(lorem1))

################################################################################
# SECTION
################################################################################

with doc.create_section(title="Imagenes"):

    ###################################
    # FRAME
    ###################################

    with doc.create_frame(title="Texto con imagen",
                          subtitle="En 1 columna ...") as frame:

        frame.append(NoEscape(lorem3))

        # Figure
        with doc.create_figure() as fig:
            path = NoEscape(os.path.join(img_dir, "sweep_0_0_0.png"))
            width = NoEscape(r'0.3\textwidth')
            fig.add_image(path, width=width)
            
    ###################################
    # FRAME
    ###################################

    with doc.create_frame(title="Texto con imagen",
                          subtitle="En 2 columnas ...") as frame:

        # Columns environment
        with doc.create_columns(alignment="T") as cols:

            # Firts column
            with doc.create_column(width=0.5) as col:

                col.title("Título de la Columna 1", "large")
                col.append(NoEscape(lorem3))
            
            # Second column
            with doc.create_column(width=0.5) as col:
                        
                col.title("Título de la Columna 2", "large")

                # Figure
                with doc.create_figure() as fig:
                    path = NoEscape(os.path.join(img_dir, "sweep_0_0_0.png"))
                    width = NoEscape(r'0.6\textwidth')
                    fig.add_image(path, width=width)
                    fig.add_caption("Tono real")

    ###################################
    # FRAME
    ###################################

    with doc.create_frame(title="Dos imagenes",
                          subtitle="En 2 columnas ...") as frame:

        # Columns environment
        with doc.create_columns(alignment="T") as cols:

            # Firts column
            with doc.create_column(width=0.33) as col:

                col.title("Caso 1", "large")
                
                # Figure
                with doc.create_figure() as fig:
                    path = NoEscape(os.path.join(img_dir, "sweep_0_0_0.png"))
                    width = NoEscape(r'\textwidth')
                    fig.add_image(path, width=width)
                    fig.add_caption("Un caso")
            
            # Second column
            with doc.create_column(width=0.33) as col:
                        
                col.title("Caso 2", "large")

                # Figure
                with doc.create_figure() as fig:
                    path = NoEscape(os.path.join(img_dir, "sweep_1_1_1.png"))
                    width = NoEscape(r'\textwidth')
                    fig.add_image(path, width=width)
                    fig.add_caption("Otro caso")

            # Thirth column
            with doc.create_column(width=0.33) as col:
                        
                col.title("Caso 3", "large")

                # Figure
                with doc.create_figure() as fig:
                    path = NoEscape(os.path.join(img_dir, "sweep_1_0_1.png"))
                    width = NoEscape(r'\textwidth')
                    fig.add_image(path, width=width)
                    fig.add_caption("Otro caso")

################################################################################
# SECTION
################################################################################

with doc.create_section(title="Itemizado"):

    ###################################
    # FRAME
    ###################################

    with doc.create_frame(title="Items e imagen",
                          subtitle="En 2 columnas ...") as frame:

        # Columns environment
        with doc.create_columns(alignment="T") as cols:

            # Firts column
            with doc.create_column(width=0.5) as col:

                col.title("Título de la Columna 1", "large")
                
                col.append("Items y enumeraciones:")

                with doc.create_itemize() as itemize:
                    itemize.add_item("Primer elemento")
                    itemize.add_item("Segundo elemento")
                    
                    # Añadir una sublista
                    with doc.create_enumerate() as subenum:
                        subenum.add_item("Subelemento ...")
                        subenum.add_item("Subelemento ...")

                    itemize.add_item("Tercer elemento")
                    itemize.add_item("Cuarto elemento")
                    
                col.append("Descripciones:")

                # Añadir una sublista
                with doc.create_description() as desc:
                    desc.add_item("Elemento 1", "descripcion ...")
                    desc.add_item("Elemento 2", "descripcion ...")
            
            # Second column
            with doc.create_column(width=0.5) as col:
                        
                col.title("Título de la Columna 2", "large")

                # Figure
                with doc.create_figure() as fig:
                    path = NoEscape(os.path.join(img_dir, "sweep_0_0_0.png"))
                    width = NoEscape(r'0.6\textwidth')
                    fig.add_image(path, width=width)
                    fig.add_caption("Otro tono real")

################################################################################
# SECTION
################################################################################

with doc.create_section(title="Otros"):

################################################################################
# SUBSECTION
################################################################################

    with doc.create_subsection(title="Matemática"):

        ###################################
        # FRAME
        ###################################

        with doc.create_frame(title="Ecuaciones con texto", 
                            subtitle="Numeracion y alineación") as frame:

            frame.append(NoEscape(lorem2))

            # Columns environment
            with doc.create_columns(alignment="T") as cols:

                # Firts column
                with doc.create_column(width=0.5) as col:
                
                    col.title("Con alineación y sin numeración")

                    with doc.create_math() as math:
                        math.append(r"a &= b + c \\")
                        math.append(r"x + y &= z \\")
                        math.append(r"e^{i\pi} + 1 &= 0 ")

                # Second column
                with doc.create_column(width=0.5) as col:
                
                    col.title("Sin alineación y con numeración")

                    with doc.create_math(numbering=True) as math:
                        math.append(r"a = b + c \\")
                        math.append(r"x + y = z \\")
                        math.append(r"e^{i\pi} + 1 = 0 ")

################################################################################
# SUBSECTION
################################################################################

    with doc.create_subsection(title="Bloques"):

        ###################################
        # FRAME
        ###################################

        with doc.create_frame(title="Bloques") as frame:

            # Blue Block
            with doc.create_blue_block("Blue Block") as bblock:
                bblock.append("Una frase")

            # Red Block
            with doc.create_red_block("Red Block") as rblock:
                rblock.append("Otra frase")

################################################################################
# SAVE
################################################################################

doc.generate('slides')