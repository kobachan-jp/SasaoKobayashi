const { minimize } = require('espresso-logic-minimizer');

const table = [
  '.i 2',
  '.o 1',
  '00 0',
  '01 1',
  '10 1',
  '11 0',
  '.e'
];

const result = minimize(table);
console.log(result);