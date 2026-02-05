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

################################################################################
# PYTHON MODULES
################################################################################

import errno
import os
import subprocess
import sys

################################################################################
# PYLATEX MODULES
################################################################################

from pylatex.base_classes import (
    Command,
    Container,
    Environment,
    LatexObject,
    UnsafeCommand,
)

from pylatex import (
    Section,
    Subsection,
    Figure,
    Command,
    Alignat,
    Itemize,
    Enumerate,
    Description
)

from pylatex.utils import (
    NoEscape, 
    dumps_list, 
    rm_temp_dir,
    bold, 
    italic
)

from pylatex.errors import CompilerError
from pylatex.package import Package
from pylatex.config import active
        
################################################################################
# BEAMER CLASS
################################################################################

class Beamer(Environment):
    r"""
    A class that contains a full LaTeX beamer.
    """

    LINE = NoEscape("\n%-------------------------------------------------------")

    def __init__(
        self,
        default_filepath="default_filepath",
        *,
        document_options=['aspectratio=169', 'english', '12pt'],
        fontenc="T1",
        inputenc="utf8",
        lmodern=True,
        textcomp=True,
        microtype=None,
        indent=None,
        data=None
    ):
        r"""
        default_filepath: str
            The default path to save files.
        documentclass: str or `~.Command`
            The LaTeX class of the document.
        document_options: str or `list`
            The options to supply to the documentclass
        fontenc: str
            The option for the fontenc package. If it is `None`, the fontenc
            package will not be loaded at all.
        inputenc: str
            The option for the inputenc package. If it is `None`, the inputenc
            package will not be loaded at all.
        lmodern: bool
            Use the Latin Modern font. This is a font that contains more glyphs
            than the standard LaTeX font.
        textcomp: bool
            Adds even more glyphs, for instance the Euro (€) sign.
        page_numbers: bool
            Adds the ability to add the last page to the document.
        indent: bool
            Determines whether or not the document requires indentation. If it
            is `None` it will use the value from the active config. Which is
            `True` by default.
        geometry_options: dict
            The options to supply to the geometry package
        data: list
            Initial content of the document.
        """

        self.default_filepath = default_filepath
        
        self.documentclass = Command(
                "documentclass", 
                arguments="beamer", 
                options=document_options
            )
        
        if indent is None:
            indent = active.indent
        if microtype is None:
            microtype = active.microtype

        # These variables are used by the __repr__ method
        self._fontenc = fontenc
        self._inputenc = inputenc
        self._lmodern = lmodern
        self._indent = indent
        self._microtype = microtype

        packages = [self.LINE]

        if fontenc is not None:
            packages.append(Package("fontenc", options=fontenc))
        if inputenc is not None:
            packages.append(Package("inputenc", options=inputenc))
        if lmodern:
            packages.append(Package("lmodern"))
        if textcomp:
            packages.append(Package("textcomp"))
        if not indent:
            packages.append(Package("parskip"))
        if microtype:
            packages.append(Package("microtype"))
        
        super().__init__(data=data)

        # Usually the name is the class name, but if we create our own
        # document class, \begin{document} gets messed up.
        self._latex_name = "document"

        self.packages |= packages
        self.variables = [self.LINE]

        self.preamble = [self.LINE]

        # No colors have been added to the document yet
        self.color = False
        self.meta_data = False

    def _propagate_packages(self):
        super()._propagate_packages()

        for item in self.preamble:
            if isinstance(item, LatexObject):
                if isinstance(item, Container):
                    item._propagate_packages()
                for p in item.packages:
                    self.packages.add(p)

    def dumps(self):
        head = self.documentclass.dumps() + "%\n"
        head += self.dumps_packages() + "%\n"
        head += dumps_list(self.variables) + "%\n"
        head += dumps_list(self.preamble) + "%\n"
        return head + "%\n" + super().dumps()

    def generate_tex(self, filepath=None):
        """Generate a .tex file for the document.

        filepath: str
            The name of the file (without .tex), if this is not supplied the
            default filepath attribute is used as the path.
        """

        super().generate_tex(self._select_filepath(filepath))

    def generate_pdf(
        self,
        filepath=None,
        *,
        clean=True,
        clean_tex=True,
        compiler=None,
        compiler_args=None,
        silent=True
    ):
        """Generate a pdf file from the document.

        filepath: str
            The name of the file (without .pdf), if it is `None` the
            ``default_filepath`` attribute will be used.
        clean: bool
            Whether non-pdf files created that are created during compilation
            should be removed.
        clean_tex: bool
            Also remove the generated tex file.
        compiler: `str` or `None`
            The name of the LaTeX compiler to use. If it is None, PyLaTeX will
            choose a fitting one on its own. Starting with ``latexmk`` and then
            ``pdflatex``.
        compiler_args: `list` or `None`
            Extra arguments that should be passed to the LaTeX compiler. If
            this is None it defaults to an empty list.
        silent: bool
            Whether to hide compiler output
        """

        if compiler_args is None:
            compiler_args = []

        # In case of newer python with the use of the cwd parameter
        # one can avoid to physically change the directory
        # to the destination folder
        python_cwd_available = sys.version_info >= (3, 6)

        filepath = self._select_filepath(filepath)
        if not os.path.basename(filepath):
            filepath = os.path.join(os.path.abspath(filepath), "default_basename")
        else:
            filepath = os.path.abspath(filepath)

        cur_dir = os.getcwd()
        dest_dir = os.path.dirname(filepath)

        if not python_cwd_available:
            os.chdir(dest_dir)

        self.generate_tex(filepath)

        if compiler is not None:
            compilers = ((compiler, []),)
        else:
            latexmk_args = ["--pdf"]

            compilers = (("latexmk", latexmk_args), ("pdflatex", []))

        main_arguments = ["--interaction=nonstopmode", filepath + ".tex"]

        check_output_kwargs = {}
        if python_cwd_available:
            check_output_kwargs = {"cwd": dest_dir}

        os_error = None

        for compiler, arguments in compilers:
            command = [compiler] + arguments + compiler_args + main_arguments

            try:
                output = subprocess.check_output(
                    command, stderr=subprocess.STDOUT, **check_output_kwargs
                )
            except (OSError, IOError) as e:
                # Use FileNotFoundError when python 2 is dropped
                os_error = e

                if os_error.errno == errno.ENOENT:
                    # If compiler does not exist, try next in the list
                    continue
                raise
            except subprocess.CalledProcessError as e:
                # For all other errors print the output and raise the error
                print(e.output.decode())
                raise
            else:
                if not silent:
                    print(output.decode())

            if clean:
                try:
                    # Try latexmk cleaning first
                    subprocess.check_output(
                        ["latexmk", "-c", filepath],
                        stderr=subprocess.STDOUT,
                        **check_output_kwargs
                    )
                except (OSError, IOError, subprocess.CalledProcessError):
                    # Otherwise just remove some file extensions.
                    extensions = ["aux", "log", "out", "fls", "fdb_latexmk"]

                    for ext in extensions:
                        try:
                            os.remove(filepath + "." + ext)
                        except (OSError, IOError) as e:
                            # Use FileNotFoundError when python 2 is dropped
                            if e.errno != errno.ENOENT:
                                raise
                rm_temp_dir()

            if clean_tex:
                os.remove(filepath + ".tex")  # Remove generated tex file

            # Compilation has finished, so no further compilers have to be
            # tried
            break

        else:
            # Notify user that none of the compilers worked.
            raise (
                CompilerError(
                    "No LaTex compiler was found\n"
                    "Either specify a LaTex compiler "
                    "or make sure you have latexmk or pdfLaTex installed."
                )
            )

        if not python_cwd_available:
            os.chdir(cur_dir)

    def _select_filepath(self, filepath):
        if filepath is None:
            return self.default_filepath
        else:
            if os.path.basename(filepath) == "":
                filepath = os.path.join(
                    filepath, os.path.basename(self.default_filepath)
                )
            return filepath

    def add_color(self, name, model, description):
        if self.color is False:
            self.packages.append(Package("color"))
            self.color = True

        self.preamble.append(
            Command("definecolor", arguments=[name, model, description])
        )

    def change_length(self, parameter, value):
        self.preamble.append(UnsafeCommand("setlength", arguments=[parameter, value]))

    def set_variable(self, name, value):
        name_arg = "\\" + name
        variable_exists = False

        for variable in self.variables:
            if name_arg == variable.arguments._positional_args[0]:
                variable_exists = True
                break

        if variable_exists:
            renew = Command(
                command="renewcommand", arguments=[NoEscape(name_arg), value]
            )
            self.append(renew)
        else:
            new = Command(command="newcommand", arguments=[NoEscape(name_arg), value])
            self.variables.append(new)

    ###########################################
    # MAIN PAGE METHODS
    ###########################################

    def title(self, title, opc=''):
        title = NoEscape(bold(title))
        self.preamble.append(Command('title', options=opc, arguments=title))

    def subtitle(self, subtitle):
        self.preamble.append(Command('subtitle', NoEscape(subtitle)))

    def author(self, author, opc=''):
        self.preamble.append(Command('author', options=opc, arguments=author))

    def institute(self, institute, opc=''):
        self.preamble.append(Command('institute', options=opc, arguments=institute))

    def date(self, date=None, opc=None):
        if date == None:
            date = NoEscape(r'\today')
        if opc == None:
            opc = NoEscape(r'\the\year')
        self.preamble.append(Command('date', options=opc, arguments=date))

    ###########################################
    # SETTINGS
    ###########################################

    def theme(self, theme='Madrid', color='default'):
        self.preamble.append(Command('usetheme', theme))
        self.preamble.append(Command('usecolortheme', color))
        self.preamble.append(Command('usefonttheme', 'professionalfonts'))

    def package(self, packages):
        if not isinstance(packages, list):
            packages = [packages]
        for package in packages:
            self.packages.append(Command('usepackage', package))

    def enable_section_frame(self, title, subtitle):
        cmd = r"\AtBeginSection[]{" + "\n" \
            + r"\begin{frame}"+ "\n" \
            + r"\frametitle{" + bold(title) + r"}" + "\n" \
            + r"\framesubtitle{" + italic(subtitle) + r"}" + "\n" \
            + r"\tableofcontents[currentsection]\end{frame}}"
        
        self.preamble.append(self.LINE)
        self.preamble.append(NoEscape(cmd))
        self.preamble.append(self.LINE)

    def enable_subsection_frame(self, title, subtitle):
        cmd = r"\AtBeginSubsection[]{" + "\n" \
            + r"\begin{frame}"+ "\n" \
            + r"\frametitle{" + bold(title) + r"}" + "\n" \
            + r"\framesubtitle{" + italic(subtitle) + r"}" + "\n" \
            + r"\tableofcontents[currentsection, currentsubsection]\end{frame}}"
        
        self.preamble.append(self.LINE)
        self.preamble.append(NoEscape(cmd))
        self.preamble.append(self.LINE)

    ###########################################
    # GENERATE
    ###########################################

    def generate(self, file_name):
        buid_path = "build"
        self.generate_tex(file_name)
        os.makedirs(os.path.abspath(buid_path), exist_ok=True)
        cmd_buid = f"latexmk -pdf -output-directory={buid_path} {file_name}.tex"
        cmd_clean = f"latexmk -c -output-directory={buid_path}"
        os.system(f"echo '{cmd_buid}' > build_tex.sh")
        os.system(f"echo '{cmd_clean}' > clean_tex.sh")

    ###########################################
    # CREATE
    ###########################################

    def create_frame(self, title='', subtitle=''):
        self.append(self.LINE)
        return self.create(Frame(title, subtitle))

    def create_section(self, title=''):
        self.append(self.LINE)
        return self.create(Section(title))
    
    def create_subsection(self, title=''):
        self.append(self.LINE)
        return self.create(Subsection(title))
    
    def create_figure(self, position='h!'):
        self.append(self.LINE)
        return self.create(Figure(position=position))
    
    def create_columns(self, alignment='c'):
        self.append(self.LINE)
        return self.create(Columns(alignment=alignment))
    
    def create_column(self, width=0.5):
        self.append(self.LINE)
        return self.create(Column(width=width))
    
    def create_math(self, aligns=2, numbering=False):
        self.append(self.LINE)
        return self.create(Alignat(aligns=aligns, numbering=numbering, escape=False))
    
    def create_itemize(self):
        self.append(self.LINE)
        return self.create(Itemize())
        
    def create_enumerate(self):
        self.append(self.LINE)
        return self.create(Enumerate())
        
    def create_description(self):
        self.append(self.LINE)
        return self.create(Description())
    
    def create_blue_block(self, title=''):
        self.append(self.LINE)
        return self.create(BlueBlock(title))

    def create_red_block(self, title=''):
        self.append(self.LINE)
        return self.create(RedBlock(title))

    ###########################################
    # .tex COMMENTS
    ###########################################
    
    def line_comment(self):
        self.append(self.LINE)

    def preamble_line_comment(self):
        self.preamble.append(self.LINE)
  
################################################################################
# FRAME CLASS
################################################################################

class Frame(Environment):
    _latex_name = 'frame'
    packages = [Package('ragged2e'), Package('microtype')]
    
    def __init__(self, title='', subtitle='', **kwargs):
        super().__init__(**kwargs)
        if title != '':
            self.title(title)
        if subtitle != '':
            self.subtitle(subtitle)
        self.justifying()

    def title(self, title):
        self.append(Command('frametitle', NoEscape(bold(title))))

    def subtitle(self, subtitle):
        self.append(Command('framesubtitle', NoEscape(italic(subtitle))))

    def title_page(self):
        self.append(NoEscape(r"\titlepage"))

    def table_of_contents(self):
        self.append(NoEscape(r"\tableofcontents"))

    def justifying(self):
        self.append(NoEscape(r"\justifying"))

################################################################################
# COLUMNS & COLUMN CLASSES
################################################################################

class Columns(Environment):
    _latex_name = 'columns'
    packages = []

    def __init__(self, alignment='c', **kwargs):
        super().__init__(options=NoEscape(alignment), **kwargs)
        self.__justifying()

    def __justifying(self):
        self.append(NoEscape(r"\justifying"))

class Column(Environment):
    _latex_name = 'column'
    packages = []

    def __init__(self, width=0.5, **kwargs):
        cmd = NoEscape(f"{width}" + r"\textwidth")
        super().__init__(arguments=NoEscape(cmd), **kwargs)
        self.justifying()

    def title(self, title, size='normalsize'):
        cmd = r'''
            \begin{minipage}{\textwidth}
                \centering
                \text{\__size__{__title__}}
            \end{minipage}        
        '''.replace('__size__', size).replace('__title__', title)
        self.append(NoEscape(cmd))

    def justifying(self):
        self.append(NoEscape(r"\justifying"))

################################################################################
# BLOCKS
################################################################################

class BlueBlock(Environment):
    _latex_name = 'block'
    packages = []

    def __init__(self, title='', **kwargs):
        super().__init__(arguments=NoEscape(title), **kwargs)

class RedBlock(Environment):
    _latex_name = 'alertblock'
    packages = []

    def __init__(self, title='', **kwargs):
        super().__init__(arguments=NoEscape(title), **kwargs)
