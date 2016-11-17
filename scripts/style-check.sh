#!/bin/bash

# Runs clang-format on the files changed between HEAD and $1, which defaults to
# origin/master.

# to pick up git-clang-format from scripts/
export PATH=$(dirname $0):$PATH

CLANG_FORMAT=${CLANG_FORMAT:-clang-format-3.8}
GITREF=${1:-origin/master}

if ! hash $CLANG_FORMAT 2> /dev/null; then
  echo "Could not find clang-format-3.8 tool" 1>&2
  exit 1
fi

cmd="git clang-format-3.8 $GITREF --binary $CLANG_FORMAT --diff --extensions h,hpp,c,cc,cpp"

n=$($cmd --quiet | grep -v "^no modified files to format" | wc -l)
if [ $n -gt 0 ]; then
  $cmd -v
  exit 1
fi
