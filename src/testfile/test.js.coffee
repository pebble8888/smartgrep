###
coffee_hello
coffee_hello
coffee_hello
###

coffee_hello

$ ->
  $("div#canvas").dblclick (e) ->
    [x, y] = positionOfNewBlock(e)
    $.post '/blocks', block: { x: x, y: y }, (block_id) ->
      block = $("<div class='block' style='left: #{x}px; top: #{y}px;' />").
      draggable(containment: "parent").css(position: "absolute")
    $(e.target).append(block)

  $("div.block").draggable(containment: "parent").css(position: "absolute")

# coffee_hello
positionOfNewBlock = (e) ->
  canvas = $(e.target)
  x = e.pageX - canvas.position().left
  y = e.pageY - canvas.position().top
  [x, y]

