version := `python3 -c "from configparser import ConfigParser; p = ConfigParser(); p.read('setup.cfg'); print(p['metadata']['version'])"`

default:
	@echo "\"make publish\"?"

tag:
	@if [ "$(git rev-parse --abbrev-ref HEAD)" != "main" ]; then exit 1; fi
	curl -H "Authorization: token `cat ~/.github-access-token`" -d '{"tag_name": "{{version}}"}' https://api.github.com/repos/nschloe/pygalmesh/releases

upload: clean
	@if [ "$(git rev-parse --abbrev-ref HEAD)" != "main" ]; then exit 1; fi
	python3 -m build --sdist .
	twine upload dist/*.tar.gz
	# HTTPError: 400 Client Error: Binary wheel 'pygalmesh-0.2.0-cp27-cp27mu-linux_x86_64.whl' has an unsupported platform tag 'linux_x86_64'. for url: https://upload.pypi.org/legacy/
	# python3 setup.py bdist_wheel
	# twine upload dist/*.whl

publish: tag upload

clean:
	@find . | grep -E "(__pycache__|\.pyc|\.pyo$\)" | xargs rm -rf
	@rm -rf build/*
	@rm -rf pygalmesh.egg-info/
	@rm -rf dist/

format:
	isort .
	black .
	blacken-docs README.md

lint:
	black --check .
	flake8 setup.py pygalmesh/ test/*.py
