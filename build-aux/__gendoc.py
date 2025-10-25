#!/usr/bin/env python3
#
#   gendoc vbeta - Generates docs from source comments
#
#   FEATURES:
#       - Generate comment-based documentation
#
#   LIMITATIONS:
#       - Platform: Only builds for POSIX systems
#
#   COMPILATION (Linux - POSIX):
#       ./gendoc.sh
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
import re
import os


def extract_blocks(source):
    pattern = re.compile(r'/\*-\s*(.*?)\s*-\*/', re.DOTALL)
    blocks = pattern.findall(source)
    results = []
    for block in blocks:
        lines = [line.strip() for line in block.strip().splitlines() if line.strip()]
        if not lines:
            continue
        decl = lines[0]
        desc = " ".join(lines[1:]) if len(lines) > 1 else ""
        results.append([decl, desc])
    return results


def search_directory(directory):
    collected = []
    for root, _, files in os.walk(directory):
        for name in files:
            if name.endswith(('.c', '.h')):
                path = os.path.join(root, name)
                with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                    collected.extend(extract_blocks(f.read()))
    return collected


def main(argv):
    source_dir = argv[1] if len(argv) > 1 else "../src"
    if not os.path.isdir(source_dir):
        print(f"error: '{source_dir}' is not a directory", file=sys.stderr)
        return 1

    result = search_directory(source_dir)
    if not result:
        return 0

    max_decl_len = max(len(decl) for decl, _ in result)
    output_path = "gendoc.html"

    header = (
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset='utf-8'>\n"
        "<title>Documentation</title>\n"
        "<style>\n"
        "body { font-family: monospace; white-space: pre; }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n<pre><code>\n"
    )

    footer = (
        "</code></pre>\n"
        "</body>\n"
        "</html>\n"
    )


    with open(output_path, 'w', encoding='utf-8') as out:
        out.write(header)
        for decl, desc in result:
            out.write(f"{decl.ljust(max_decl_len)}  // {desc}\n")
        out.write(footer)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
