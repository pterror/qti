/** @param {string} text */
export function extractWords(text) {
  return text.match(/\S+/g);
}

/** @param {readonly string[]} words */
export function gamesSearchQuery(words, hasWhere = false) {
  return words.length === 0
    ? ""
    : (hasWhere ? " AND " : " WHERE ") +
        words
          .map(
            (word) =>
              "(title LIKE '%" +
              word +
              "%' OR short_text LIKE '%" +
              word +
              "%')"
          )
          .join(" AND ");
}

/** @param {readonly string[]} words */
export function collectionsSearchQuery(words, hasWhere = false) {
  return words.length === 0
    ? ""
    : (hasWhere ? " AND " : " WHERE ") +
        words.map((word) => "title LIKE '%" + word + "%'").join(" AND ");
}
