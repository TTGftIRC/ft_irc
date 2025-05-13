#!/bin/bash

PORT=8080  # change this if needed

PID=$(lsof -ti tcp:$PORT)

if [ -n "$PID" ]; then
  echo "Killing process using port $PORT (PID: $PID)"
  kill -9 $PID
else
  echo "Port $PORT is free"
fi
