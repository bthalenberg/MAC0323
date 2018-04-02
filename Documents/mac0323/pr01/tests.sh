# Rodar casos testes

## Caso teste do PDF
macsim ep1.mac 65 < case_0.in > case_0.out

## Caso teste 1.1 do enunciado
macsim ep1.mac 80 < case_1.in > case_1_1.out

## Caso teste 1.2 do enunciado
macsim ep1.mac 150 < case_1.in > case_1_2.out

## Caso teste 2.1 do enunciado
macsim ep1.mac 80 < case_2.in > case_2_1.out

## Caso teste 2.2 do enunciado
macsim ep1.mac 150 < case_2.in > case_2_2.out

## Caso teste 3 (small)
macsim ep1.mac 40 < case_3.in > case_3.out

## Caso teste 4 (small with word truncation)
macsim ep1.mac 10 < case_3.in > case_5.out

## Caso teste 6 (paragraph)
macsim ep1.mac 80 < case_5.in > case_6.out

## Caso teste 7 (paragraph)
macsim ep1.mac 80 < case_5.in > case_6.out

## Caso teste 5 (medium)
macsim ep1.mac 60 < case_4.in > case_4.out
