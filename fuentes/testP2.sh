#!/bin/bash

echo -e "\n\x1b[32mEJ 1:\x1b[0m" 
echo -e "\nCrear sala nombre: Sala1.txt y capacidad: 20"
./misala crea -f Sala1.txt -c 20
./misala estado -f Sala1.txt

echo "---------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 2:\x1b[0m" 
echo -e "\nVuelta a crear sin -o"
./misala crea -f Sala1.txt -c 20

echo "---------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 3:\x1b[0m"
echo -e "\nVuelta a crear con -o y cambiando capacidad a 25"
./misala crea -f Sala1.txt -c 25 -o
./misala estado -f Sala1.txt

echo "----------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 4:\x1b[0m"
echo -e "\nReservamos 10 asientos con ids -> 1, 2, 3, 4, 5, 6, 7, 8, 9, 10"
./misala reserva -f Sala1.txt -n 10 1 2 3 4 5 6 7 8 9 10
./misala estado -f Sala1.txt

echo "-----------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 5:\x1b[0m"
echo -e "\nAnulamos asientos 3 y 6"
./misala anula -f Sala1.txt -a 3 6
./misala estado -f Sala1.txt

echo "-----------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 6:\x1b[0m"
echo -e "\nAnulamos asientos con ids 8 y 10"
./misala anula -f Sala1.txt -i 8 10
./misala estado -f Sala1.txt

echo "-----------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 7:\x1b[0m"
echo -e "\nCreamos otro archivo para comparar con nombre Sala2.txt y capacidad 20"
echo -e "RESULTADO -> Debe dar distinto\n"
./misala crea -f Sala2.txt -c 20 -o

echo -e "\nEstado Sala 1:"
./misala estado -f Sala1.txt

echo -e "\nEstado Sala 2:"
./misala estado -f Sala2.txt

./misala compara Sala1.txt Sala2.txt

echo "-----------------------------------------------------------------"
echo -e "\n\x1b[32mEJ 8:\x1b[0m"
echo -e "\nSobreescribimos el archivo Sala1.txt para dejarlo todo a 0"
./misala crea -f Sala1.txt -c 20 -o

echo -e "\nEstado Sala 1:"
./misala estado -f Sala1.txt

echo -e "\nEstado Sala 2:"
./misala estado -f Sala2.txt

./misala compara Sala1.txt Sala2.txt

echo "-----------------------------------------------------------------"
echo -e "\nReservamos en ambas salas lo mismo y comparamos"
./misala reserva -f Sala1.txt -n 10 1 2 3 4 5 6 7 8 9 10
./misala reserva -f Sala2.txt -n 10 1 2 3 4 5 6 7 8 9 10

echo -e "\nEstado Sala 1:"
./misala estado -f Sala1.txt

echo -e "\nEstado Sala 2:"
./misala estado -f Sala2.txt

./misala compara Sala1.txt Sala2.txt


