# Querier Implementation
## Author: Chikwanda Chisha   
Date: October, 2024

### Implementation Details
 
 \
**Below is the pseudocode  each component of querier.c**
#### 1. `main`
- Parse and validate command-line arguments (`pageDirectory`, `indexFilename`)
    - If arguments are invalid:
        - Print error and exit
    - If `pageDirectory` is not a crawler directory:
        - Print error and exit
- Load `index` from `indexFilename`
    - If index load fails:
        - Print error and exit
- While there is input:
    - Prompt for query (if interactive)
    - Read query
    - Process query
- Clean up and exit

#### 2. `process_query`
- Clean and normalize input query (`clean_query`)
    - If `clean_query` is invalid:
        - Print error and return
- Tokenize `clean_query` into an array of words (`tokens`)
    - If tokens are invalid:
        - Print error and return
- Evaluate the tokenized query (`results`)
    - If results exist:
        - Print ranked results
- Free allocated memory

#### 3. `clean_input`
- Allocate memory for cleaned query
- For each character in the query:
    - If alphabetic:
        - Add lowercase version to `cleaned`
    - Else if space:
        - Add single space (avoid duplicates)
    - Else:
        - Print error and return `null`
- Return cleaned query

#### 4. `tokenize`
- Count words in query
- Allocate array for tokens
- Split query on spaces:
    - For each word:
        - Allocate memory
        - Copy word
        - Add to array
- Validate token syntax
- Return array or `null` if error

#### 5. `evaluate_query`
- Initialize result counters
- For each token:
    - If `or`:
        - Start new term
    - Else if `and` or implicit `and`:
        - Intersect with current term
    - Else:
        - Get word counters from index
        - Update current result
- Return final scored results

#### 6. `print_results`
- Count non-zero scores
- Allocate score array
- Fill array with `docID`, `score` pairs
- Sort array by decreasing score
- For each result:
    - Get URL from `pageDirectory`
    - Print `score`, `docID`, `URL`
- Free allocated memory

### Testing Plan

#### Query Syntax Tests
- Test invalid characters
- Test operator placement rules
- Test blank queries
- Test case sensitivity

#### Functionality Tests
- Single-word queries
- Multi-word implicit `AND` queries
- Explicit `AND`/`OR` queries
- Complex combinations
- Edge cases (words not in index)

#### Memory Testing
- Use Valgrind to check for leaks
- Use Valgrind to check for invalid memory access
- Test cleanup on error conditions




