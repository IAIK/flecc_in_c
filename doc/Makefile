## please modify following line for naming the end products (PDFs)
PROJECTNAME = "flecc_in_c_user_docu"

MAINDOCUMENTBASENAME = "flecc_in_c_user_docu"

## -----------------------------------------
##       DO NOT EDIT BELOW THIS LINE
## -----------------------------------------

## Makefile von Karl Voit (Karl@Voit.net)

## some Makefile-hints taken from: 
## http://www.ctan.org/tex-archive/help/uk-tex-faq/Makefile

DATESTAMP = `/bin/date +%Y-%m-%d`

## file names
DATESTAMP_AND_PROJECT = ${DATESTAMP}_${PROJECTNAME}
MAINDOCUMENTFILENAME = ${MAINDOCUMENTBASENAME}.tex

DOT_NATIVE = $(wildcard figures_native/*.dot)
DOT_PDF    = $(patsubst figures_native/%.dot,figures/%.pdf,$(DOT_NATIVE))

IPE_NATIVE = $(wildcard figures_native/*.ipe)
IPE_PDF    = $(patsubst figures_native/%.ipe,figures/%.pdf,$(IPE_NATIVE))

OO_NATIVE  = $(wildcard figures_native/*.odg)
OO_PDF     = $(patsubst figures_native/%.odg,figures/%.pdf,$(OO_NATIVE))

## commands
PDFLATEX_CMD = pdflatex
BIBTEX_CMD = bibtex
#BIBTEX_CMD = biber

#help
#helpThe main targets of this Makefile are:
#help	help	this help
.PHONY: help
help:
	@sed -n 's/^#help//p' < Makefile

#help	all	see "pdf"
.PHONY: all
all: pdf

figures/%.pdf: figures_native/%.dot
	dot -Tpdf -o $@ $<
	pdfcrop $@ $@-cropped
	mv $@-cropped $@
	
figures/%.pdf: figures_native/%.ipe
	ipetoipe -pdf $< $@

figures/%.pdf: figures_native/%.odg
	loffice --headless --convert-to pdf --outdir figures $<
	pdfcrop $@ $@-cropped
	mv $@-cropped $@

#help	assets	generate pdf figures from native file formats (dot, ipe, odg)
assets: $(DOT_PDF) $(IPE_PDF) $(OO_PDF)

#help	pdf	makes a file per pdflatex
.PHONY: pdf
pdf: assets
	${PDFLATEX_CMD} -interaction=nonstopmode ${MAINDOCUMENTFILENAME}
	-${BIBTEX_CMD} ${MAINDOCUMENTBASENAME}
	${PDFLATEX_CMD} -interaction=nonstopmode ${MAINDOCUMENTFILENAME}
	${PDFLATEX_CMD} -interaction=nonstopmode ${MAINDOCUMENTFILENAME}
	${PDFLATEX_CMD} -interaction=nonstopmode ${MAINDOCUMENTFILENAME}
	cp ${MAINDOCUMENTBASENAME}.pdf ${DATESTAMP_AND_PROJECT}.pdf

	
#help	wc	counts the words from the PDF generated
wc:	pdf
	pdftops ${DATESTAMP_AND_PROJECT}.pdf
	ps2ascii ${DATESTAMP_AND_PROJECT}.ps > ${DATESTAMP_AND_PROJECT}.txt
	wc -w ${DATESTAMP_AND_PROJECT}.txt

# --------------------------------------------------------


#help	clean	clean up temporary files
.PHONY: clean
clean: 
	-rm -f ${MAINDOCUMENTBASENAME}.pdf
	-rm -f *~ *.backup *.bak *.synctex.gz
	-rm -f *.aux *.toc *.log *.lof *.lot
	-rm -f *.bbl *.bcf *.blg *-blx.bib *.brf *.run.xml
	-rm -f *.nav *.out *.snm *.vrb

#help	purge	cleaner than clean ;-)
.PHONY: purge
purge: clean
	-rm -f *.pdf

#help	force	force rebuild next run
.PHONY: force
force:
	touch *.tex

#end
