../../src/sog-its -SDSOG -R3f -C -c -g -f"A U B" exemple2.net > dsog.dot
../../src/sog-its -SSOG -R3f -C -c -g -f"A U B" exemple2.net > sog.dot
../../src/sog-its -SSOG -R3f -C -c -g -f"1" exemple2.net > sog-pur.dot
../../src/sog-its -SSLOG -R3f -C -c -g -f"A U B" exemple2.net > slog.dot
../../src/sog-its -SSLOG-FST -R3f -C -c -g -f"A U B" exemple2.net > slog-fst.dot
../../src/sog-its -SDSOG -R3f -C  -c  -s -f"A U B" exemple2.net 2> formula.dot
name=sog ; dot -Tpdf $name.dot -o $name.pdf
name=sog-pur ; dot -Tpdf $name.dot -o $name.pdf
name=dsog ; dot -Tpdf $name.dot -o $name.pdf
name=slog ; dot -Tpdf $name.dot -o $name.pdf
name=formula ; dot -Tpdf $name.dot -o $name.pdf
name=slog-fst ; dot -Tpdf $name.dot -o $name.pdf

