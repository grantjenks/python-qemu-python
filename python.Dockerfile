FROM python:3.11.4-slim-bookworm

RUN rm -f /sbin/init
COPY init /sbin/init
