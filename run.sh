#!/usr/bin/env bash
# run.sh - build-free launcher for the Tiny Search Engine demo.
# Serves a local site over HTTP/1.1, crawls it, indexes it, then opens the querier.
#
# Usage:  ./run.sh [siteDir] [maxDepth]
#   siteDir   folder to serve & crawl (default: bigsite; try mock_website)
#   maxDepth  crawl depth 0..10       (default: 2)
#
# Examples:
#   ./run.sh                     # crawl bigsite at depth 2
#   ./run.sh mock_website        # crawl the small 3-page site
#   ./run.sh bigsite 3           # crawl bigsite at depth 3
set -euo pipefail

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

SITE="${1:-bigsite}"
DEPTH="${2:-2}"
PORT=8080
NAME="$(basename "$SITE")"
PAGEDIR="data/$NAME"
INDEX="data/$NAME.index"
SEED="http://localhost:${PORT}/index.html"

if [ ! -f "$DIR/$SITE/index.html" ]; then
  echo "error: '$SITE/index.html' not found. Available sites:" >&2
  for d in "$DIR"/*/index.html; do [ -f "$d" ] && echo "  - $(basename "$(dirname "$d")")" >&2; done
  exit 1
fi

# --- start local HTTP/1.1 server (crawler rejects HTTP/1.0) ---
# Background python DIRECTLY (not in a subshell) so $! is python's own PID and
# the trap can actually kill it. cd is a bash builtin, so it handles /c/... paths.
cd "$DIR/$SITE"
python -c "
import http.server, socketserver
http.server.SimpleHTTPRequestHandler.protocol_version = 'HTTP/1.1'
socketserver.TCPServer(('127.0.0.1', ${PORT}), http.server.SimpleHTTPRequestHandler).serve_forever()
" &
SRV=$!
cd "$DIR"
trap 'kill "$SRV" 2>/dev/null || true' EXIT

# wait until the server answers
for _ in $(seq 1 20); do
  if curl -s -m 2 -o /dev/null "$SEED"; then break; fi
  sleep 0.3
done

echo "=== crawl $SITE (depth $DEPTH) ==="
mkdir -p "$PAGEDIR"
rm -f "$PAGEDIR"/[0-9]*
./crawler/crawler.exe "$SEED" "$PAGEDIR" "$DEPTH"

echo "=== index ==="
./indexer/indexer.exe "$PAGEDIR" "$INDEX"

echo "=== querier (type words, e.g. 'coffee' or 'chess and music'; Ctrl-D to quit) ==="
./querier/querier.exe "$PAGEDIR" "$INDEX"
