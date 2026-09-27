import asyncio, sys, re
from playwright.async_api import async_playwright
files = sys.argv[1:]
async def main():
    async with async_playwright() as p:
        b = await p.chromium.launch()
        pg = await b.new_page(device_scale_factor=2)
        for f in files:
            svg = open(f).read()
            w = int(re.search(r'width="(\d+)"', svg).group(1)); h = int(re.search(r'height="(\d+)"', svg).group(1))
            await pg.set_viewport_size({"width": w, "height": h})
            await pg.set_content(f'<html><body style="margin:0">{svg}</body></html>')
            await pg.screenshot(path=f.replace(".svg", ".png"), clip={"x":0,"y":0,"width":w,"height":h})
        await b.close()
asyncio.run(main())
