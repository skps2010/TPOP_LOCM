SID = 40747039s

all:
	yapf -ri --style='{based_on_style: pep8, column_limit: 66}' .
	clang-format -i $(shell find . -type f -name "*.cpp") \
					$(shell find . -type f -name "*.h") \
					$(shell find . -type f -name "*.c")
	
	cd code && make
	python3 make_c.py
	python3 auto_report.py
	doc2pdf test.docx
	mv test.pdf $(SID).pdf
	rm test.docx
	zip $(SID).zip -r $(shell \
		git ls-tree -r \
			$(shell git rev-parse --abbrev-ref HEAD) \
		--name-only)
clean:
	cd code && make clean
	rm -rf $(SID).pdf $(SID).zip Answer.cpp