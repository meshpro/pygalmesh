VERSION=$(shell grep '__version__ = ' setup.py | sed 's/[^0-9]*\([0-9\.]*\).*/\1/')

# Make sure we're on the master branch
ifneq "$(shell git rev-parse --abbrev-ref HEAD)" "master"
$(error Not on master branch)
endif

default:
	@echo "\"make publish\"?"

README.rst: README.md
	pandoc README.md -o README.rst
	python setup.py check -r -s || exit 1

upload: setup.py README.rst
	python setup.py sdist upload --sign
	# HTTPError: 400 Client Error: Binary wheel 'frentos-0.2.0-cp27-cp27mu-linux_x86_64.whl' has an unsupported platform tag 'linux_x86_64'. for url: https://upload.pypi.org/legacy/
	# rm -f dist/*
	# python setup.py bdist_wheel --universal
	# gpg --detach-sign -a dist/*
	# twine upload dist/*

tag:
	@echo "Tagging v$(VERSION)..."
	git tag v$(VERSION)
	git push --tags

publish: tag upload

clean:
	rm -f README.rst
