# IMPLEMENTATION.md - Querier Implementation

## Implementation Details

### 1. `main`
- Validates command-line inputs and loads the inverted index.

### 2. `process_query`
- Cleans and validates the query.
- Tokenizes the query by splitting on spaces.
- Evaluates query using boolean logic and scoring rules.

### 3. `evaluate_query`
- Processes tokens, using counters for `and` and `or` operations.
- Uses `counters_intersect` and `counters_union` to manage scoring.

## Testing Plan
- **Syntax validation**: Queries with syntax errors should return informative error messages.
- **Functionality tests**: Test single-word queries, multi-word queries with `and`/`or`, edge cases.
- **Memory testing**: Use Valgrind to confirm no memory leaks.

### Known Issues
Any known bugs should be documented here, along with planned fixes.

*Author: Chikwanda Chisha*
*Date: 30 October, 2024*
