BIN  = whtgen

.PHONY: all
all: $(BIN)

utils.cmi: utils.mli
	ocamlc -c $<

utils.cmx : utils.ml utils.cmi
	ocamlopt -c $<

wht.cmi : wht.mli
	ocamlc -c $<

wht.cmx : wht.ml utils.cmi wht.cmi
	ocamlopt -c $<

interleave.cmi : interleave.mli
	ocamlc -c $<

interleave.cmx : interleave.ml wht.cmi utils.cmi interleave.cmi
	ocamlopt -c $<

vectorize.cmi : vectorize.mli
	ocamlc -c $<

vectorize.cmx : vectorize.ml wht.cmi utils.cmi vectorize.cmi
	ocamlopt -c $<

whtgen.cmx : whtgen.ml utils.cmx wht.cmx interleave.cmx vectorize.cmx
	ocamlopt -c $<

$(BIN): utils.cmx wht.cmx interleave.cmx vectorize.cmx whtgen.cmx
	ocamlopt -o $@ utils.cmx wht.cmx interleave.cmx vectorize.cmx whtgen.cmx

.PHONY: clean
clean:
	rm -f $(BIN) *.cmi *.cmx *.o

