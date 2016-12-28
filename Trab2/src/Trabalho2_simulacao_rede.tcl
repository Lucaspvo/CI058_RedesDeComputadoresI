#Trabalho 2 - Redes I
#Jessiel Heitor Hacke 
#Lucas Pazelo Vargas de Oliveira 
#-----------------------------

#Source
source source.tcl

#-------- Variaveis --------#

# Saída para NAM
#set saidaNAM out.nam

# Número de switches
set numSwitchesFilhos 6

# Número de máquinas por switch
set maquinasPorSwitch 10

# Velocidade dos links da rede
set velocidadeRede 10Mb

# Delay dos links da rede
set delayRede 10ms

# Número de Receptores
set receptores 10

# Número de Transmissores
set transmissores 60

# Tempo de simulação, em segundos
set tempo 60.0

#--------DEFINE A REDE-------------------#

# cria objeto simulador
set ns [new Simulator]

# abre o arquivo de trace
set traceOut [open $saidaNS w]
$ns trace-all $traceOut

# abre o arquivo trace do nam
#set nf [open $saidaNAM w]
#$ns namtrace-all $nf

# define a função finish
proc finish {} {
    #    global ns nf
        global ns
        $ns flush-trace
    #    close $nf
    #    exec nam $saidaNAM &
        exit 0
}

#cria switch pai

set pai [$ns node]

# cria os switches filhos
for {set i 0} {$i < $numSwitchesFilhos} {incr i} {
        set switchesFilhos($i) [$ns node]
        $switchesFilhos($i) color Blue
        $switchesFilhos($i) shape box
}

# liga os switches filhos ao pai e pendura as maquinas neles
for {set i 0} {$i < $numSwitchesFilhos} {incr i} {
        $ns duplex-link $switchesFilhos($i) $pai $velocidadeRede $delayRede DropTail
        for {set x 0} {$x < $maquinasPorSwitch} {incr x} {
                #gera o numero da maquina que fai no switch
                set numMaquinas [expr ($i * $maquinasPorSwitch + $x)]

                set maquinas($numMaquinas) [$ns node]
                $ns duplex-link $switchesFilhos($i) $maquinas($numMaquinas) $velocidadeRede $delayRede DropTail
        }
}

set totalMaquinas [expr $maquinasPorSwitch * $numSwitchesFilhos]

set i 0

# define os transmissores
while {$i < $transmissores} {
                set udp($i) [new Agent/UDP]
                $ns attach-agent $maquinas($i) $udp($i)
                set cbr($i) [new Application/Traffic/CBR]
                $cbr($i) set interval_ $interval
                $cbr($i) attach-agent $udp($i)
                incr i
}

# define os receptores aleatoriamente
set i 0
while {$i < $receptores} {
        set num [expr { int($totalMaquinas * rand()) }]
        if { [info exists arrTR($num)] } {
 
        } else {
               set arrTR($num) 1
               set null($i) [new Agent/Null]
               $ns attach-agent $maquinas($num) $null($i)

               incr i
        }
}

# conecta os transmissores aos receptores.
for {set i 0} {$i < $transmissores} {incr i} {
        set indice [expr { $i % $receptores} ]
        $ns connect $udp($i) $null($indice)
}

# inicia a transmissão
for {set i 0} {$i < $transmissores} {incr i} {
    $ns at 0.0 "$cbr($i) start"
    $ns at $tempo "$cbr($i) stop"
}

# chama "finish" após "tempo" segundos de simulação
$ns at $tempo "finish"

$ns run
