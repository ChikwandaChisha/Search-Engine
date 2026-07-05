#!/usr/bin/env bash
# mirror.sh - download a real (static) website into sites/<name>/ and rewrite its
# links to http://localhost:8080/ so the Tiny Search Engine can crawl it offline.
#
# Usage:  ./mirror.sh URL [siteName] [downloadDepth]
#   URL            root URL to mirror, e.g. https://quotes.toscrape.com/
#   siteName       folder under sites/ (default: the host name)
#   downloadDepth  wget recursion depth (default: 2)
#
# After mirroring, crawl it with:   ./run.sh sites/<name> <crawlDepth>
#
# NOTE: only works for plain static sites (no JS-rendered content). Be polite:
# use small sites or shallow depth; this is meant for scraping-friendly demos.
set -euo pipefail

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
URL="${1:?usage: ./mirror.sh URL [siteName] [downloadDepth]}"
HOST="$(printf '%s' "$URL" | sed -E 's#^https?://##; s#/.*$##')"     # host[:port]
NAME="${2:-$HOST}"
DEPTH="${3:-2}"
OUT="$DIR/sites/$NAME"

# wget lives in MSYS2; make it reachable even when launched from Git Bash
command -v wget >/dev/null 2>&1 || export PATH="$PATH:/c/msys64/usr/bin"
command -v wget >/dev/null || { echo "wget not found (run this from the MSYS2 shell, or add C:\\msys64\\usr\\bin to PATH)" >&2; exit 1; }

echo ">> mirroring $URL  (host=$HOST, depth=$DEPTH) -> sites/$NAME"
rm -rf "$OUT"; mkdir -p "$OUT"

# -r -l DEPTH : recursive to given depth      -nH         : no host dir
# --no-parent : stay within the seed's path   --domains   : stay on this host
# --reject    : skip heavy assets (links stay in HTML; crawler just 404-skips them)
# NO --adjust-extension / --convert-links: keep filenames matching URL paths so the
# local server resolves them, and leave absolute links so we can rewrite the host.
wget -e robots=off -r -l "$DEPTH" --no-parent -nH -P "$OUT" \
     --domains "$HOST" \
     --reject 'css,js,png,jpg,jpeg,gif,svg,ico,woff,woff2,ttf,eot' \
     -q --show-progress "$URL" || true   # wget exits non-zero on any 404; that's fine

# rewrite absolute links to the real host -> localhost:8080
mapfile -d '' FILES < <(grep -rlZ -e "http://$HOST" -e "https://$HOST" "$OUT" 2>/dev/null || true)
if [ "${#FILES[@]}" -gt 0 ]; then
  sed -i "s#https\{0,1\}://$HOST#http://localhost:8080#g" "${FILES[@]}"
fi

PAGES="$(find "$OUT" -type f \( -name '*.html' -o -name '*.htm' -o -not -name '*.*' \) | wc -l)"
echo ">> done: sites/$NAME  (~$PAGES pages)"
[ -f "$OUT/index.html" ] || echo "!! warning: no index.html at sites/$NAME root; run.sh seeds /index.html"
echo ">> now run:   ./run.sh sites/$NAME 2"
