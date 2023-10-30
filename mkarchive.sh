#!/bin/sh

RELEASE_NAME_FILE=`dirname $0`/latest_release
RELEASE_NAME=`cat ${RELEASE_NAME_FILE}`

git archive --prefix="${RELEASE_NAME}"/ -o "${RELEASE_NAME}"_source.tgz "${RELEASE_NAME}"
