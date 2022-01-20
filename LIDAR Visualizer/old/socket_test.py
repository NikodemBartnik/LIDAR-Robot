import asyncio
import websockets

async def echo(websocket):
    async for message in websocket:
        print(message)
        await websocket.send("left")

async def main():
    async with websockets.serve(echo, "192.168.0.107", 8084):
        await asyncio.Future()  # run forever

asyncio.run(main())