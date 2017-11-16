# Intro

This project is a 1:1 port of the Python blockchain implementation in Daniel van Flymen's article

  [Learn Blockchains by Building One](https://hackernoon.com/learn-blockchains-by-building-one-117428612f46)

* Comments are taken from original implementation
* Class and function names match original where appropriate, except for naming convention (`CamelCase` instead of `snake_case`)

[![Build Status](https://travis-ci.org/bbiskup/blockchain-dvf.svg)](https://travis-ci.org/bbiskup/blockchain-dvf)

# Installation
## System Requirements

* Docker >= 17.05.0
* docker-compose >= 1.13.0

## Building the project for the first time

    $ ./bootstrap.sh

## Ongoing development

Start a docker container:

    $ docker-compose up -d

Then, enter a container shell:

    $ ./docker-cmd.sh bash
    $ cd build

Compile:

    $ ninja

Run tests:

    $ ./tests

## Running tests

(to be run outside of the Docker container)

    $ ./run-tests.sh

# Using the API

First, start blockchain server in container:

    $ docker-compose up -d
    $ ./docker-cmd.sh ./build/blockchain-dvf    

`docker-compose.yml` maps the container port 5000 to port 5000 on the host.
To reset the blockchain, interrupt the server with `CTRL-c` and start it again.

## Mining a block

    $ curl http://localhost:5000/mine

## Creating a new transaction

    $ curl -X POST --header "Content-Type:application/json" \
           --data '{"sender": "abc", "recipient": "def", "amount": 10}' \
           http://localhost:5000/transactions/new

## Retrieving block chain

    $ curl http://localhost:5000/chain

## Registering a new node

    $ curl -X POST --header "Content-Type:application/json" \
           --data '{"nodes": ["http://localhost:5001"]}' \
           http://localhost:5000/nodes/register

## Resolving chain conflicts (achieving consensus)

    $ curl http://localhost:5000/nodes/resolve
