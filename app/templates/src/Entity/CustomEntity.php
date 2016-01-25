<?php
/**
 * Created by PhpStorm.
 * User: tomfriedhof
 * Date: 1/11/16
 * Time: 12:10 PM
 */

namespace Drupal\custom_entity\Entity;


class CustomEntity extends \Entity {

  public function __construct(array $values, $entityType) {
    parent::__construct($values, $entityType);
    $this->type = 'custom_entity';
  }

  public function uri() {
    return [
      'path' => 'custom_entity/' . $this->id
    ];
  }

}