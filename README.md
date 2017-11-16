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

## Building the project

    $ ./bootstrap.sh

## Running tests

    $ ./run-tests.sh

# Using the API

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
