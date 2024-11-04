# DESIGN.md - Querier Design

*Author: Chikwanda Chisha *
*Date: 30 October, 2024 *

## Purpose
The querier is responsible for reading queries from `stdin`, parsing them, and retrieving relevant documents from the inverted index produced by the indexer. Queries support basic boolean operations (`and`, `or`) with precedence for `and`.

## Data Structures
- **index_t**: A data structure for storing the inverted index with terms mapped to document counters.
- **counters_t**: Stores document ID and occurrence count pairs for each query term.

## Pseudocode
1. **Main Function**
   - Parse command-line arguments, validate inputs.
   - Load index from file into `index_t`.
   - Read and process queries in an interactive loop.

2. **Process Query**
   - Clean query string: normalize case, remove invalid characters.
   - Tokenize query and evaluate terms using boolean precedence.
   - Collect results, rank them by score, and display.

3. **Evaluate Query**
   - Process `and` sequences, combining counters by taking the minimum score.
   - For `or` sequences, combine results by summing scores for matching documents.

## Scoring
- Each document receives a score based on term frequency, summed for `or` terms and minimized for `and` terms.

