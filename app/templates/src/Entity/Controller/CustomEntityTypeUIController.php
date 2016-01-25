<?php

namespace Drupal\custom_entity\Entity\Controller;


/**
 * UI controller.
 */
class CustomEntityTypeUIController extends \EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Custom Entity, including fields.';
    return $items;
  }
}