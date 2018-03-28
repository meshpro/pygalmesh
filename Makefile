VERSION=$(shell python3 -c "import pygalmesh; print(pygalmesh.__version__)")

default:
	@echo "\"make publish\"?"

tag:
	@if [ "$(shell git rev-parse --abbrev-ref HEAD)" != "master" ]; then exit 1; fi
	@echo "Tagging v$(VERSION)..."
	git tag v$(VERSION)
	git push --tags

upload: setup.py
	# Make sure we're on the master branch
	@if [ "$(shell git rev-parse --abbrev-ref HEAD)" != "master" ]; then exit 1; fi
	rm -rf dist/*
	python3 setup.py sdist
	# HTTPError: 400 Client Error: Binary wheel 'pygalmesh-0.2.0-cp27-cp27mu-linux_x86_64.whl' has an unsupported platform tag 'linux_x86_64'. for url: https://upload.pypi.org/legacy/
	# python3 setup.py bdist_wheel --universal
	gpg --detach-sign -a dist/*
	twine upload dist/*.tar.gz
	# twine upload dist/*.whl

publish: tag upload
