#!/bin/bash

echo "🚀 Testando múltiplos clientes TCP"

# Compilar se necessário
cd build
make clean
make

# Iniciar servidor em background
echo "📡 Iniciando servidor..."
./tcp_server &
SERVER_PID=$!

sleep 2  # Aguardar servidor inicializar

# Iniciar múltiplos clientes em background
echo "👥 Iniciando clientes..."

for i in {1..3}; do
    echo "Cliente $i iniciado"
    (echo -e "Mensagem do cliente $i\nOutra mensagem $i\nsair" | ./tcp_client) &
    sleep 1
done

# Aguardar um pouco
sleep 5

# Finalizar servidor
kill $SERVER_PID

echo "✅ Teste concluído. Verifique server.log"
