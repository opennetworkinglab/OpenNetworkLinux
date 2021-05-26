VERSION=1.0
USER=opennetworking
REPO=onl-builder-buster

TOOLS=../../../tools/docker_shell ../../../tools/container-id

build:
	cp $(TOOLS) .
	docker build -t $(USER)/$(REPO):$(VERSION) .
	rm -rf $(notdir $(TOOLS))
