FROM node:20-alpine

WORKDIR /app


COPY package.json .
RUN npm install


COPY webpack.config.js .
COPY server.js .
COPY src/ ./src/
COPY public/ ./public/


RUN npm run build

EXPOSE 3000

CMD ["node", "server.js"]
