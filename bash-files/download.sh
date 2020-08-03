#!/bin/bash
mkdir /tmp/final_project
curl -o /tmp/final_project/files.gz.tar https://loh.istgahesalavati.ir/report.gz.tar
tar -xvf /tmp/final_project/files.gz.tar -C /tmp/final_project/
gcc -o ~/Desktop/fifth-internship-project/fifth-internship-project/main ~/Desktop/fifth-internship-project/fifth-internship-project/main.c -I/Applications/Postgres.app/Contents/Versions/12/include -lpq -std=c99
~/Desktop/fifth-internship-project/fifth-internship-project/main 
rm /tmp/final_project/*
