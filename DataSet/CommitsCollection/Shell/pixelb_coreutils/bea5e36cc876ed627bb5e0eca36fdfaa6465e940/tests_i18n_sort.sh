#!/bin/sh
# Verify sort's multi-byte support.

. "${srcdir=.}/tests/init.sh"; path_prepend_ ./src
print_ver_ sort

export LC_ALL=en_US.UTF-8
locale -k LC_CTYPE | grep -q "charmap.*UTF-8" \
  || skip_ "No UTF-8 locale available"

# Enable heap consistency checkng on older systems
export MALLOC_CHECK_=2


# check buffer overflow issue due to
# expanding multi-byte representation due to case conversion
# https://bugzilla.suse.com/show_bug.cgi?id=928749
cat <<EOF > exp
.
ɑ
EOF
cat <<EOF | sort -f > out || fail=1
.
ɑ
EOF
compare exp out || { fail=1; cat out; }


Exit $fail
