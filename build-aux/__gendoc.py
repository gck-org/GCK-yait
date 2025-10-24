#
#   gendoc vbeta - Generates docs from source comments
#
#   FEATURES:
#       - Generate complete build systems: Autotools-like Makefile
#       - Generate complete base system, ready to compile
#       - Generate preconfigured gcklib, ready to import
#
#   LIMITATIONS:
#       - Platform: Only builds for POSIX systems
#
#   COMPILATION (Linux - POSIX):
#       ./gendoc.sh
#
#
#   LICENSE: BSD-3-Clause
#
#   Copyright (c) 2025 GCK
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import sys
import os

header = '''
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<style>
</style>
</head>
<body>
<div class="header"></div>
<div class="reference">
<pre><code>
'''

footer = '''
</code></pre>
</div>
</body>
</html>
'''

entry = '''
%s // %s
'''

# TODO: read all files
# TODO: Parse for /*- -*/
# TODO: Add entry to entries arr

def main(args):
    print(f"call {args[0]}")
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
