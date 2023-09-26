# SQL DB

In-memory relational database with support for simplified SQL queries.

## Task

A simplified relational database with support for “stripped-down” sql has been implemented. The work is done in the form of a library that contains the SQLDB class, which has a public method for executing the query.

### Supported syntax

Keywords:

- SELECT
- FROM
- WHERE
- (LEFT|RIGHT|INNER)JOIN
- CREATE TABLE
- DROP TABLE
- AND
- OR
- IS
- NOT
- NULL
- ON
- UPDATE
- INSERT
- VALUES
- DELETE
- PRIMARY KEY
- FOREIGN KEY

Supported data types:

- bool
- int
- double
- varchar

Restrictions:

- nested subqueries are not supported
- Join only for 2 tables

## Implementation

Only memory is used to store data.

No external libraries are used.
