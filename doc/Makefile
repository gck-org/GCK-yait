hello.info: hello.texi version.texi
	makeinfo --no-split hello.texi -o hello.info

hello.html: hello.texi version.texi
	makeinfo --no-split --html hello.texi

hello.pdf: hello.texi version.texi
	makeinfo --pdf hello.texi

hello.txt: hello.texi version.texi
	makeinfo --plaintext hello.texi -o hello.txt

clean:
	rm -f *.aux *.cp *.cps *.fn *.fns *.ky *.kys *.log *.pg *.pgs *.toc *.tp *.tps *.vr *.vrs
	rm -f hello.info hello.dvi hello.pdf hello.html hello.txt

install: hello.info
	install -d $(DESTDIR)$(infodir)
	install -m 644 hello.info $(DESTDIR)$(infodir)/
	install-info --info-dir=$(DESTDIR)$(infodir) $(DESTDIR)$(infodir)/hello.info

.PHONY: all clean install
