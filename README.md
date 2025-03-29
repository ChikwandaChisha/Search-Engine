# Search Engine 

## Author: Chikwanda Chisha (ChikwandaChisha)

A robust search engine implementation that crawls web pages, indexes content, and provides search functionality. 

## Features

- Web crawler that traverses web pages within a specified depth
- Indexer that processes and stores page content
- Query interface for searching indexed pages
- Efficient data structures for storing and retrieving information

## Project Structure

- `crawler/`: Web crawler implementation
- `indexer/`: Indexer implementation
- `querier/`: Query interface implementation
- `common/`: Shared code and utilities
- `lib/`: Library implementations

## Setup

1. Clone the repository:
```bash
git clone https://github.com/ChikwandaChisha/Search-Engine.git
cd Search-Engine
```

2. Build the project:
```bash
make
```

## Usage

### Crawler
```bash
./crawler/crawler [seedURL] [pageDirectory] [maxDepth]
```

### Indexer
```bash
./indexer/indexer [pageDirectory] [indexFile]
```

### Querier
```bash
./querier/querier [indexFile] [pageDirectory]
```

## Implementation Notes

- The crawler respects robots.txt and stays within the same domain
- The indexer creates an inverted index for efficient searching
- The querier supports AND/OR operations in queries
- All components include comprehensive error handling and logging

## Testing

Run the test suite:
```bash
make test
```



