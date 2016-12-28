set title 'Grafico Taxa'
set ylabel 'Taxa(%)'
set xlabel 'Mensagens/s'
set yrange [0:100]
set autoscale
set terminal png
set output 'Grafico_Taxa.png'
plot 'Taxa.out' with lines
