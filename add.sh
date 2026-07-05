#!/usr/bin/env bash
# add.sh - paste a URL, get it crawled and searchable in one step.
#
# Usage:  ./add.sh URL [name] [crawlDepth]
#   URL         any static website URL, e.g. https://www.planetary.org/worlds
#   name        folder under sites/ (default: derived from the URL)
#   crawlDepth  how deep the crawler follows links (default: 1)
#
# What it does:
#   1. mirrors the URL's section into sites/<name>/  (bounded, skips heavy assets)
#   2. rewrites the site's links to http://localhost:8080/
#   3. builds an index.html hub linking to every downloaded page
#   4. serves + crawls + indexes it, then opens the querier
#
# Only works on static (server-rendered) sites; JavaScript-only apps come down empty.
set -euo pipefail
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# wget lives in MSYS2; make it reachable even when launched from Git Bash
command -v wget >/dev/null 2>&1 || export PATH="$PATH:/c/msys64/usr/bin"

URL="${1:?usage: ./add.sh URL [name] [crawlDepth]}"

# --- parse the URL into host + path ---
noscheme="${URL#http://}"; noscheme="${noscheme#https://}"
HOST="${noscheme%%/*}"
if [ "$noscheme" = "$HOST" ]; then PATHPART="/"; else PATHPART="/${noscheme#*/}"; fi

# bound the mirror to the URL's section so it can't sprawl the whole site
case "$PATHPART" in
  "/")     INCLUDE="" ;;               # whole site (depth + quota are the only guard)
  */*/*)   INCLUDE="${PATHPART%/*}" ;; # /a/b/c -> /a/b
  *)       INCLUDE="$PATHPART" ;;      # /worlds -> /worlds
esac

# default folder name: host (minus www/TLD) + first path segment
seg="${PATHPART#/}"; seg="${seg%%/*}"
DEFAULT="$(printf '%s' "$HOST" | sed -E 's/^www\.//; s/\.[a-z.]+$//')"
[ -n "$seg" ] && DEFAULT="$DEFAULT-$seg"
NAME="${2:-$DEFAULT}"
DEPTH="${3:-1}"
OUT="$DIR/sites/$NAME"

command -v wget >/dev/null || { echo "need wget (run from the MSYS2 shell, or add C:\\msys64\\usr\\bin to PATH)" >&2; exit 1; }

echo ">> mirroring $URL"
echo "   host=$HOST  section=${INCLUDE:-<whole site>}  ->  sites/$NAME"
rm -rf "$OUT"; mkdir -p "$OUT"

INCL=(); [ -n "$INCLUDE" ] && INCL=(--include-directories="$INCLUDE")
wget -e robots=off -r -l 2 --no-parent -nH -P "$OUT" --domains "$HOST" \
     "${INCL[@]}" --quota=100m \
     --reject 'css,js,png,jpg,jpeg,gif,svg,ico,woff,woff2,ttf,eot,webp,avif,mp4,mp3,pdf,zip' \
     -q --show-progress "$URL" || true   # non-zero on any 404 is fine

# rewrite absolute links on this host -> localhost:8080
mapfile -d '' FS < <(grep -rlZ -e "http://$HOST" -e "https://$HOST" "$OUT" 2>/dev/null || true)
[ "${#FS[@]}" -gt 0 ] && sed -i "s#https\{0,1\}://$HOST#http://localhost:8080#g" "${FS[@]}"

# build an index.html hub linking to every downloaded HTML page (guarantees crawl coverage)
{
  echo "<!DOCTYPE html><html><head><title>$NAME</title></head><body>"
  echo "<h1>$NAME</h1><p>Local mirror of $URL</p><ul>"
  while IFS= read -r f; do
    rel="${f#"$OUT"/}"
    [ "$rel" = "index.html" ] && continue
    case "$rel" in *[!A-Za-z0-9._/-]*) continue ;; esac   # skip junk/query-string names
    if head -c 512 "$f" | grep -qiE '<!doctype html|<html'; then
      echo "  <li><a href=\"$rel\">$rel</a></li>"
    fi
  done < <(find "$OUT" -type f | sort)
  echo "</ul></body></html>"
} > "$OUT/index.html"

pages="$(grep -c '<li>' "$OUT/index.html" || true)"
echo ">> built sites/$NAME hub: $pages pages"
if [ "${pages:-0}" -eq 0 ]; then
  echo "!! no static HTML pages found - the site is probably JavaScript-rendered and can't be crawled this way." >&2
  exit 1
fi

# hand off to the existing serve + crawl + index + query pipeline
exec "$DIR/run.sh" "sites/$NAME" "$DEPTH"
