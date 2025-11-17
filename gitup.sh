#!/bin/bash
# Usage: ./upgit.sh "message 1" "message 2" "message 3"

commitMessage=""
for msg in "$@"; do
    commitMessage="$commitMessage -m \"$msg\""
done

git add -A
git status
eval "git commit $commitMessage"
git push
