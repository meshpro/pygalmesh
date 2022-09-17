# version := `python3 -c "import tomli; c = tomli.load(open('pyproject.toml', 'rb')); print(c['project']['version'])"`
version := `python3 -c "from configparser import ConfigParser; p = ConfigParser(); p.read('setup.cfg'); print(p['metadata']['version'])"`

default:
	@echo "\"make publish\"?"

tag:
	@if [ "$(git rev-parse --abbrev-ref HEAD)" != "main" ]; then exit 1; fi
	curl -H "Authorization: token `cat ~/.github-access-token`" -d '{"tag_name": "v{{version}}"}' https://api.github.com/repos/nschloe/pygalmesh/releases

upload: clean
	@if [ "$(git rev-parse --abbrev-ref HEAD)" != "main" ]; then exit 1; fi
	python3 -m build --sdist .
	twine upload dist/*.tar.gz

publish: tag upload

clean:
	@find . | grep -E "(__pycache__|\.pyc|\.pyo$)" | xargs rm -rf
	@rm -rf src/*.egg-info/ build/ dist/ .tox/ pygalmesh.egg-info// builddir/

format:
	isort .
	black .
	blacken-docs README.md

lint:
	black --check .
	flake8 setup.py pygalmesh/ tests/*.py
